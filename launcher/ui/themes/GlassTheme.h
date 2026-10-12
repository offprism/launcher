// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "FusionTheme.h"

class QWidget;

// OffPrism: dark, semi-transparent theme. Top level windows get a translucent background,
// on Windows 11 the system acrylic backdrop is shown behind them.
class GlassTheme : public FusionTheme {
   public:
    virtual ~GlassTheme() {}

    void apply(bool initial) override;

    QString id() override;
    QString name() override;
    QString tooltip() override;
    bool hasStyleSheet() override;
    QString appStyleSheet() override;
    QPalette colorScheme() override;
    double fadeAmount() override;
    QColor fadeColor() override;

    //! True while the glass theme is the active application theme
    static bool isActive();
    //! Called by ITheme::apply for every other theme
    static void deactivate();
    //! Makes a top level window translucent (must be called before it is shown for the first time)
    static void makeTranslucent(QWidget* window);
    //! Sets the palette of popups and input widgets, which are never translucent
    static void setPopupPalette(const QPalette& palette);
};
