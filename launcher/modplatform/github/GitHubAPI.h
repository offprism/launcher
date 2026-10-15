// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "modplatform/ModIndex.h"
#include "modplatform/ResourceAPI.h"

// OffPrism: mods, resource packs and shader packs published as GitHub releases.
// Repositories are found by their GitHub topics, every release becomes a version and
// the release asset matching the resource type becomes the downloaded file.
class GitHubAPI final : public ResourceAPI {
   public:
    static const GitHubAPI& get()
    {
        static const GitHubAPI s_instance;
        return s_instance;
    }

    auto getSortingMethods() const -> QList<ResourceAPI::SortingMethod> override;

    std::pair<Task::Ptr, QByteArray*> getProjects(QStringList addonIds) const override;

    Net::RPC::Spec<ModPlatform::IndexedPack> getProject(const QString& id) const override;
    Net::RPC::Spec<QList<ModPlatform::IndexedPack>> searchProjects(const SearchArgs& args) const override;

    auto getVersionsURL(const VersionSearchArgs& args) const -> std::optional<QString> override;
    auto getDependencyURL(const DependencySearchArgs& args) const -> std::optional<QString> override;

    Result<ModPlatform::IndexedVersion> loadIndexedPackVersion(QJsonObject& obj, ModPlatform::ResourceType type) const override;
    QList<ModPlatform::IndexedVersion> loadIndexedPackVersions(QJsonObject& obj, ModPlatform::ResourceType type) const override;

    std::pair<Task::Ptr, QByteArray*> getModCategories() const override;
    QList<ModPlatform::Category> loadModCategories(const QByteArray& response) const override;

    //! GitHub topic used to find repositories of the given resource type
    static QString topicFor(ModPlatform::ResourceType type);

   protected:
    QString debugName() const override { return "GitHub"; }
};
