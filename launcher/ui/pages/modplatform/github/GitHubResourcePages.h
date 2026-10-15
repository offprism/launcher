// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "ui/pages/modplatform/ModPage.h"
#include "ui/pages/modplatform/ResourcePackPage.h"
#include "ui/pages/modplatform/ShaderPackPage.h"

namespace ResourceDownload {

// OffPrism: mods, resource packs and shader packs from GitHub releases
namespace GitHub {
static inline QString displayName()
{
    return "GitHub";
}
static inline QIcon icon()
{
    return QIcon::fromTheme("github");
}
static inline QString id()
{
    return "github";
}
static inline QString debugName()
{
    return "GitHub";
}
static inline QString metaEntryBase()
{
    return "GitHubPacks";
}

ShaderPackResourcePage* createShaderPackResourcePage(ResourceDownloadDialog* dialog, BaseInstance& instance);
ResourcePackResourcePage* createResourcePackResourcePage(ResourceDownloadDialog* dialog, BaseInstance& instance);
ModPage* createModPage(ResourceDownloadDialog* dialog, BaseInstance& instance);
}  // namespace GitHub

}  // namespace ResourceDownload
