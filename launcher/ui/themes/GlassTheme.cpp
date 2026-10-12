// SPDX-License-Identifier: GPL-3.0-only
#include "GlassTheme.h"

#include <QApplication>
#include <QDialog>
#include <QEvent>
#include <QMainWindow>
#include <QObject>
#include <QWidget>

#ifdef Q_OS_WIN
#include <dwmapi.h>
#include <windows.h>
#endif

namespace {
constexpr double s_windowOpacity = 0.93;
bool s_glassActive = false;

bool isGlassWindow(QWidget* widget)
{
    return widget && widget->isWindow() && (qobject_cast<QMainWindow*>(widget) || qobject_cast<QDialog*>(widget));
}

void styleWindow(QWidget* window, bool glass)
{
    window->setWindowOpacity(glass ? s_windowOpacity : 1.0);
#ifdef Q_OS_WIN
    if (glass && window->testAttribute(Qt::WA_WState_Created)) {
        // Dark title bar to match the theme (DWMWA_USE_IMMERSIVE_DARK_MODE)
        BOOL dark = TRUE;
        DwmSetWindowAttribute(reinterpret_cast<HWND>(window->winId()), 20, &dark, sizeof(dark));
    }
#endif
}

class GlassWindowFilter : public QObject {
   public:
    using QObject::QObject;

   protected:
    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (event->type() == QEvent::Show && GlassTheme::isActive()) {
            auto* widget = qobject_cast<QWidget*>(watched);
            if (isGlassWindow(widget))
                styleWindow(widget, true);
        }
        return QObject::eventFilter(watched, event);
    }
};

GlassWindowFilter* s_filter = nullptr;
}  // namespace

bool GlassTheme::isActive()
{
    return s_glassActive;
}

void GlassTheme::deactivate()
{
    if (!s_glassActive)
        return;
    s_glassActive = false;
    for (auto* widget : QApplication::topLevelWidgets()) {
        if (isGlassWindow(widget))
            styleWindow(widget, false);
    }
}

void GlassTheme::apply(bool initial)
{
    FusionTheme::apply(initial);
    s_glassActive = true;
    if (!s_filter) {
        s_filter = new GlassWindowFilter(qApp);
        qApp->installEventFilter(s_filter);
    }
    for (auto* widget : QApplication::topLevelWidgets()) {
        if (isGlassWindow(widget) && widget->isVisible())
            styleWindow(widget, true);
    }
}

QString GlassTheme::id()
{
    return "glass";
}

QString GlassTheme::name()
{
    return QObject::tr("Glass (translucent)");
}

QString GlassTheme::tooltip()
{
    return QObject::tr("Dark theme with semi-transparent windows.");
}

QPalette GlassTheme::colorScheme()
{
    QPalette p;
    p.setColor(QPalette::Window, QColor(30, 32, 40));
    p.setColor(QPalette::WindowText, QColor(236, 238, 244));
    p.setColor(QPalette::Base, QColor(22, 24, 30));
    p.setColor(QPalette::AlternateBase, QColor(28, 30, 37));
    p.setColor(QPalette::ToolTipBase, QColor(36, 38, 48));
    p.setColor(QPalette::ToolTipText, QColor(236, 238, 244));
    p.setColor(QPalette::Text, QColor(236, 238, 244));
    p.setColor(QPalette::Button, QColor(44, 47, 58));
    p.setColor(QPalette::ButtonText, QColor(236, 238, 244));
    p.setColor(QPalette::BrightText, QColor(255, 90, 90));
    p.setColor(QPalette::Link, QColor(120, 190, 255));
    p.setColor(QPalette::Highlight, QColor(110, 132, 230));
    p.setColor(QPalette::HighlightedText, Qt::white);
    p.setColor(QPalette::PlaceholderText, QColor(150, 154, 166));
    return fadeInactive(p, fadeAmount(), fadeColor());
}

double GlassTheme::fadeAmount()
{
    return 0.5;
}

QColor GlassTheme::fadeColor()
{
    return QColor(30, 32, 40);
}

bool GlassTheme::hasStyleSheet()
{
    return true;
}

QString GlassTheme::appStyleSheet()
{
    return "QToolTip { color: #eceef4; background-color: #242630; border: 1px solid #4a4e5c; }";
}
