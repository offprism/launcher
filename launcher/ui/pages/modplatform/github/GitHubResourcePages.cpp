// SPDX-License-Identifier: GPL-3.0-only
#include "GitHubResourcePages.h"

#include "minecraft/MinecraftInstance.h"
#include "modplatform/github/GitHubAPI.h"
#include "ui/dialogs/ResourceDownloadDialog.h"

namespace ResourceDownload {

static ResourceProviderData prepareGitHub()
{
    return {
        .displayName = GitHub::displayName(),
        .icon = GitHub::icon(),
        .id = GitHub::id(),
        .metaEntryBase = GitHub::metaEntryBase(),
        .debugName = GitHub::debugName(),
    };
}

ShaderPackResourcePage* GitHub::createShaderPackResourcePage(ResourceDownloadDialog* dialog, BaseInstance& instance)
{
    return new ShaderPackResourcePage(dialog, instance, prepareGitHub(), &GitHubAPI::get());
}

ResourcePackResourcePage* GitHub::createResourcePackResourcePage(ResourceDownloadDialog* dialog, BaseInstance& instance)
{
    return new ResourcePackResourcePage(dialog, instance, prepareGitHub(), &GitHubAPI::get());
}

ModPage* GitHub::createModPage(ResourceDownloadDialog* dialog, BaseInstance& instance)
{
    return new ModPage(dialog, instance, prepareGitHub(), &GitHubAPI::get(),
                       ModFilterWidget::create(&static_cast<MinecraftInstance&>(instance), false));
}
}  // namespace ResourceDownload
