// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "FusionTheme.h"

class QWidget;

// OffPrism: dark semi-transparent theme. Widgets are painted opaque (so nothing can be left over on screen),
// the whole top level window is then made slightly transparent by the window system.
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
    //! Called by ITheme::apply for every theme, restores opaque windows
    static void deactivate();
};
