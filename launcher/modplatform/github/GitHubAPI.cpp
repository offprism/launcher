// SPDX-License-Identifier: GPL-3.0-only
#include "GitHubAPI.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QUrl>
#include <QUrlQuery>

#include "Application.h"
#include "net/ApiRequest.h"
#include "net/NetJob.h"

namespace {
const QString s_api = "https://api.github.com";

ModPlatform::IndexedPack packFromRepo(const QJsonObject& repo)
{
    ModPlatform::IndexedPack pack;
    auto fullName = repo.value("full_name").toString();
    auto owner = repo.value("owner").toObject();

    pack.addonId = fullName;
    pack.provider = ModPlatform::ResourceProvider::GITHUB;
    pack.name = repo.value("name").toString();
    pack.slug = fullName;
    pack.description = repo.value("description").toString();
    pack.authors.append({ .name = owner.value("login").toString(), .url = owner.value("html_url").toString() });
    pack.logoUrl = owner.value("avatar_url").toString();
    pack.logoName = QString("github_%1.png").arg(QString(fullName).replace('/', '_'));
    pack.websiteUrl = repo.value("html_url").toString();

    pack.extraDataLoaded = true;
    pack.extraData.sourceUrl = pack.websiteUrl;
    pack.extraData.issuesUrl = pack.websiteUrl + "/issues";
    auto homepage = repo.value("homepage").toString();
    if (!homepage.isEmpty())
        pack.extraData.wikiUrl = homepage;
    pack.extraData.body = QObject::tr("%1\n\n⭐ %2 · [%3](%4)")
                              .arg(pack.description)
                              .arg(repo.value("stargazers_count").toInt())
                              .arg(fullName, pack.websiteUrl);
    return pack;
}

bool isWantedAsset(const QString& name, ModPlatform::ResourceType type)
{
    auto lower = name.toLower();
    if (type == ModPlatform::ResourceType::Mod) {
        static const QRegularExpression s_extra("-(sources|javadoc|dev|api)\\.jar$");
        // server plugins and standalone tools that are published next to the mod
        static const QRegularExpression s_notMod("(^|[-_.])(cli|paper|spigot|bukkit|sponge|velocity|bungee|bungeecord|folia|server|plugin)([-_.]|$)");
        return lower.endsWith(".jar") && !s_extra.match(lower).hasMatch() && !s_notMod.match(lower).hasMatch();
    }
    return lower.endsWith(".zip");
}

// Minecraft versions mentioned in a file name or tag, e.g. "mymod-fabric-1.21.4-2.0.jar"
QStringList guessMcVersions(const QString& text)
{
    static const QRegularExpression s_mc("(?<![\\d.])1\\.(\\d{1,2})(?:\\.(\\d{1,2}))?(?![\\d])");
    QStringList found;
    auto it = s_mc.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        // the minor version of Minecraft releases is between 0 and 21+, ignore things like "1.0" mod versions
        if (m.captured(1).toInt() < 2)
            continue;
        auto ver = m.captured(0);
        if (!found.contains(ver))
            found.append(ver);
    }
    return found;
}

ModPlatform::ModLoaderTypes guessLoaders(const QString& text)
{
    auto lower = text.toLower();
    ModPlatform::ModLoaderTypes loaders;
    if (lower.contains("neoforge"))
        loaders |= ModPlatform::NeoForge;
    else if (lower.contains("forge"))
        loaders |= ModPlatform::Forge;
    if (lower.contains("fabric"))
        loaders |= ModPlatform::Fabric;
    if (lower.contains("quilt"))
        loaders |= ModPlatform::Quilt;
    return loaders;
}
}  // namespace

QString GitHubAPI::topicFor(ModPlatform::ResourceType type)
{
    switch (type) {
        case ModPlatform::ResourceType::ShaderPack:
            return "minecraft-shaders";
        case ModPlatform::ResourceType::ResourcePack:
        case ModPlatform::ResourceType::TexturePack:
            return "minecraft-resourcepack";
        case ModPlatform::ResourceType::DataPack:
            return "minecraft-datapack";
        default:
            return "minecraft-mod";
    }
}

QList<ResourceAPI::SortingMethod> GitHubAPI::getSortingMethods() const
{
    return { { .index = 1, .name = "stars", .readableName = QObject::tr("Sort by Stars") },
             { .index = 2, .name = "updated", .readableName = QObject::tr("Sort by Last Updated") },
             { .index = 3, .name = "", .readableName = QObject::tr("Sort by Relevance") } };
}

Net::RPC::Spec<QList<ModPlatform::IndexedPack>> GitHubAPI::searchProjects(const SearchArgs& args) const
{
    // https://docs.github.com/rest/search/search#search-repositories
    QString query = QString("topic:%1").arg(topicFor(args.type));
    if (args.search.has_value() && !args.search->trimmed().isEmpty())
        query = args.search->trimmed() + " " + query;

    QUrlQuery params;
    params.addQueryItem("q", query);
    params.addQueryItem("per_page", "25");
    params.addQueryItem("page", QString::number(args.offset / 25 + 1));
    auto sort = args.sorting.has_value() ? args.sorting->name : QString("stars");
    if (!sort.isEmpty()) {
        params.addQueryItem("sort", sort);
        params.addQueryItem("order", "desc");
    }
    QUrl url(s_api + "/search/repositories");
    url.setQuery(params);

    return { { .url = url }, [](const QByteArray& response) -> Result<QList<ModPlatform::IndexedPack>> {
                QJsonParseError error;
                auto doc = QJsonDocument::fromJson(response, &error);
                if (error.error != QJsonParseError::NoError || !doc.isObject())
                    return std::unexpected(QObject::tr("GitHub returned an invalid response: %1").arg(error.errorString()));
                QList<ModPlatform::IndexedPack> packs;
                for (const auto& item : doc.object().value("items").toArray()) {
                    packs.append(packFromRepo(item.toObject()));
                }
                return packs;
            } };
}

Net::RPC::Spec<ModPlatform::IndexedPack> GitHubAPI::getProject(const QString& id) const
{
    return { { .url = QUrl(s_api + "/repos/" + id) }, [](const QByteArray& response) -> Result<ModPlatform::IndexedPack> {
                auto doc = QJsonDocument::fromJson(response);
                if (!doc.isObject())
                    return std::unexpected(QObject::tr("GitHub returned an invalid response."));
                return packFromRepo(doc.object());
            } };
}

std::pair<Task::Ptr, QByteArray*> GitHubAPI::getProjects(QStringList addonIds) const
{
    auto netJob = makeShared<NetJob>(QString("GitHub::GetProjects"), APPLICATION->network());
    auto [action, response] = Net::ApiRequest::makeByteArray(QUrl(s_api + "/repos/" + addonIds.value(0)));
    netJob->addNetAction(action);
    return { netJob, response };
}

auto GitHubAPI::getVersionsURL(const VersionSearchArgs& args) const -> std::optional<QString>
{
    return QString("%1/repos/%2/releases?per_page=50").arg(s_api, args.pack->addonId.toString());
}

auto GitHubAPI::getDependencyURL([[maybe_unused]] const DependencySearchArgs& args) const -> std::optional<QString>
{
    // GitHub releases don't describe dependencies
    return {};
}

namespace {
QList<QJsonObject> wantedAssets(const QJsonObject& release, ModPlatform::ResourceType type)
{
    QList<QJsonObject> assets;
    if (release.value("draft").toBool())
        return assets;
    for (const auto& assetValue : release.value("assets").toArray()) {
        auto asset = assetValue.toObject();
        if (isWantedAsset(asset.value("name").toString(), type))
            assets.append(asset);
    }
    return assets;
}

ModPlatform::IndexedVersion versionFromAsset(const QJsonObject& release, const QJsonObject& asset, ModPlatform::ResourceType type, bool several)
{
    auto tag = release.value("tag_name").toString();
    auto releaseName = release.value("name").toString().isEmpty() ? tag : release.value("name").toString();
    auto fileName = asset.value("name").toString();

    ModPlatform::IndexedVersion file;
    file.fileId = asset.value("id").toVariant();
    // a release with a file per mod loader shows the file name, so the entries can be told apart
    file.version = several ? QString("%1 (%2)").arg(releaseName, fileName) : releaseName;
    file.versionNumber = tag;
    file.versionType = ModPlatform::IndexedVersionType::fromString(release.value("prerelease").toBool() ? "Beta" : "Release");
    file.mcVersion = guessMcVersions(fileName + " " + tag);
    file.downloadUrl = asset.value("browser_download_url").toString();
    file.date = release.value("published_at").toString();
    file.fileName = fileName;
    if (type == ModPlatform::ResourceType::Mod) {
        // the loader of each file is taken from its own name, the tag only helps single-file releases
        file.loaders = guessLoaders(several ? fileName : fileName + " " + tag);
    }
    file.changelog = release.value("body").toString();

    // GitHub publishes a SHA-256 digest for release assets ("sha256:<hex>")
    auto digest = asset.value("digest").toString();
    if (digest.startsWith("sha256:")) {
        file.hashType = "sha256";
        file.hash = digest.mid(7);
    }
    return file;
}
}  // namespace

Result<ModPlatform::IndexedVersion> GitHubAPI::loadIndexedPackVersion(QJsonObject& release, ModPlatform::ResourceType type) const
{
    auto assets = wantedAssets(release, type);
    if (assets.isEmpty())
        return std::unexpected(QString("release has no suitable file"));
    return versionFromAsset(release, assets.first(), type, assets.size() > 1);
}

QList<ModPlatform::IndexedVersion> GitHubAPI::loadIndexedPackVersions(QJsonObject& release, ModPlatform::ResourceType type) const
{
    QList<ModPlatform::IndexedVersion> versions;
    auto assets = wantedAssets(release, type);
    for (const auto& asset : assets) {
        versions.append(versionFromAsset(release, asset, type, assets.size() > 1));
    }
    return versions;
}

std::pair<Task::Ptr, QByteArray*> GitHubAPI::getModCategories() const
{
    // GitHub has no categories; an empty job finishes immediately
    static QByteArray s_empty;
    auto netJob = makeShared<NetJob>(QString("GitHub::GetCategories"), APPLICATION->network());
    return { netJob, &s_empty };
}

QList<ModPlatform::Category> GitHubAPI::loadModCategories([[maybe_unused]] const QByteArray& response) const
{
    return {};
}
