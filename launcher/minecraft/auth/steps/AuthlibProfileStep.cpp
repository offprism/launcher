#include "AuthlibProfileStep.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "Application.h"

AuthlibProfileStep::AuthlibProfileStep(AccountData* data) : AuthStep(data) {}

QString AuthlibProfileStep::describe()
{
    return tr("Fetching profile skin.");
}

void AuthlibProfileStep::perform()
{
    auto root = m_data->authlibServer();
    while (root.endsWith('/'))
        root.chop(1);
    QUrl url(QString("%1/sessionserver/session/minecraft/profile/%2?unsigned=true").arg(root, m_data->minecraftProfile.id));

    auto [request, response] = Net::Request::makeByteArray(url);
    m_request = request;

    m_task.reset(new NetJob("AuthlibProfileStep", APPLICATION->network()));
    m_task->setAskRetry(false);
    m_task->addNetAction(m_request);

    connect(m_task.get(), &Task::finished, this, [this, response] { onRequestDone(response); });

    m_task->start();
}

void AuthlibProfileStep::onRequestDone(QByteArray* response)
{
    // A missing skin is not an error, the default one will be used
    if (m_request->error() == QNetworkReply::NoError) {
        auto properties = QJsonDocument::fromJson(*response).object().value("properties").toArray();
        for (const auto& propertyV : properties) {
            auto property = propertyV.toObject();
            if (property.value("name").toString() != "textures")
                continue;
            auto texturesJson = QByteArray::fromBase64(property.value("value").toString().toLatin1());
            auto textures = QJsonDocument::fromJson(texturesJson).object().value("textures").toObject();
            auto skin = textures.value("SKIN").toObject();
            if (!skin.isEmpty()) {
                m_data->minecraftProfile.skin.id = "authlib";
                m_data->minecraftProfile.skin.url = skin.value("url").toString();
                m_data->minecraftProfile.skin.variant =
                    skin.value("metadata").toObject().value("model").toString() == "slim" ? "SLIM" : "CLASSIC";
            }
        }
    }
    emit finished(AccountTaskState::STATE_WORKING, tr("Got profile"));
}
