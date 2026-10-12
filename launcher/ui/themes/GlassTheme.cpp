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
QPalette solidPalette();
const char* const s_opaqueClasses[] = { "QMenu", "QComboBoxPrivateContainer", "QToolTip", "QLineEdit", "QComboBox",
                                         "QAbstractSpinBox", "QPlainTextEdit", "QTextEdit" };
}  // namespace

namespace {
bool s_glassActive = false;

class GlassWindowFilter : public QObject {
   public:
    using QObject::QObject;

   protected:
    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (event->type() == QEvent::Polish && GlassTheme::isActive()) {
            auto* widget = qobject_cast<QWidget*>(watched);
            if (widget && widget->isWindow() && (qobject_cast<QMainWindow*>(widget) || qobject_cast<QDialog*>(widget))) {
                GlassTheme::makeTranslucent(widget);
            }
        }
        return QObject::eventFilter(watched, event);
    }
};

GlassWindowFilter* s_filter = nullptr;

#ifdef Q_OS_WIN
void enableBackdrop(QWidget* window)
{
    auto hwnd = reinterpret_cast<HWND>(window->winId());
    // Let DWM draw behind the whole client area
    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    // Dark title bar (DWMWA_USE_IMMERSIVE_DARK_MODE)
    BOOL dark = TRUE;
    DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));

    // Acrylic backdrop on Windows 11 22H2+ (DWMWA_SYSTEMBACKDROP_TYPE = 38, DWMSBT_TRANSIENTWINDOW = 3).
    // Older systems ignore the attribute and just keep the semi-transparent window.
    int backdrop = 3;
    DwmSetWindowAttribute(hwnd, 38, &backdrop, sizeof(backdrop));
}
#endif
}  // namespace

bool GlassTheme::isActive()
{
    return s_glassActive;
}

void GlassTheme::deactivate()
{
    s_glassActive = false;
}

void GlassTheme::setPopupPalette(const QPalette& palette)
{
    for (const char* cls : s_opaqueClasses) {
        QApplication::setPalette(palette, cls);
    }
}

void GlassTheme::makeTranslucent(QWidget* window)
{
    if (window->testAttribute(Qt::WA_TranslucentBackground))
        return;
    window->setAttribute(Qt::WA_TranslucentBackground);
#ifdef Q_OS_WIN
    enableBackdrop(window);
#endif
}

void GlassTheme::apply(bool initial)
{
    FusionTheme::apply(initial);
    auto solid = fadeInactive(solidPalette(), fadeAmount(), fadeColor());
    setPopupPalette(solid);
    s_glassActive = true;
    if (!s_filter) {
        s_filter = new GlassWindowFilter(qApp);
        qApp->installEventFilter(s_filter);
    }
    // windows that already exist (when switching themes at runtime)
    for (auto* widget : QApplication::topLevelWidgets()) {
        if (!widget->isVisible() && (qobject_cast<QMainWindow*>(widget) || qobject_cast<QDialog*>(widget)))
            makeTranslucent(widget);
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
    return QObject::tr("Dark semi-transparent theme. Restart the launcher after selecting it to make all windows translucent.");
}

namespace {
// Opaque dark palette (same spirit as the Dark theme), used for controls and popups
QPalette solidPalette()
{
    QPalette p;
    p.setColor(QPalette::Window, QColor(38, 40, 48));
    p.setColor(QPalette::WindowText, QColor(236, 238, 244));
    p.setColor(QPalette::Base, QColor(28, 30, 36));
    p.setColor(QPalette::AlternateBase, QColor(34, 36, 43));
    p.setColor(QPalette::ToolTipBase, QColor(30, 32, 40));
    p.setColor(QPalette::ToolTipText, QColor(236, 238, 244));
    p.setColor(QPalette::Text, QColor(236, 238, 244));
    p.setColor(QPalette::Button, QColor(52, 55, 66));
    p.setColor(QPalette::ButtonText, QColor(236, 238, 244));
    p.setColor(QPalette::BrightText, QColor(255, 90, 90));
    p.setColor(QPalette::Link, QColor(120, 190, 255));
    p.setColor(QPalette::Highlight, QColor(110, 132, 230));
    p.setColor(QPalette::HighlightedText, Qt::white);
    p.setColor(QPalette::PlaceholderText, QColor(150, 154, 166));
    return p;
}
}  // namespace

QPalette GlassTheme::colorScheme()
{
    // Only the window and view backgrounds are translucent, everything Fusion draws with gradients stays opaque
    QPalette palette = solidPalette();
    palette.setColor(QPalette::Window, QColor(22, 24, 30, 185));
    palette.setColor(QPalette::Base, QColor(14, 16, 20, 150));
    palette.setColor(QPalette::AlternateBase, QColor(255, 255, 255, 12));
    return fadeInactive(palette, fadeAmount(), fadeColor());
}

double GlassTheme::fadeAmount()
{
    return 0.5;
}

QColor GlassTheme::fadeColor()
{
    return QColor(38, 40, 48);
}

bool GlassTheme::hasStyleSheet()
{
    return true;
}

QString GlassTheme::appStyleSheet()
{
    // translucent windows do not fill their background by themselves
    return "QMainWindow, QDialog { background-color: rgba(22, 24, 30, 185); }\n"
           "QStatusBar { background-color: rgba(22, 24, 30, 200); }\n"
           "QToolTip { color: #eceef4; background-color: #1e2028; border: 1px solid #4a4e5c; }";
}
