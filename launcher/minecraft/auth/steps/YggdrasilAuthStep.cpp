#include "YggdrasilAuthStep.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QUuid>

#include "Application.h"
#include "net/NetUtils.h"
#include "net/RawHeaderProxy.h"
#include "net/Request.h"

YggdrasilAuthStep::YggdrasilAuthStep(AccountData* data, bool refresh) : AuthStep(data), m_refresh(refresh) {}

QString YggdrasilAuthStep::serviceName() const
{
    if (m_data->type == AccountType::ElyBy) {
        return "Ely.by";
    }
    return QUrl(m_data->authlibServer()).host();
}

QString YggdrasilAuthStep::authserverUrl() const
{
    if (m_data->type == AccountType::ElyBy) {
        return "https://authserver.ely.by/auth";
    }
    auto root = m_data->authlibServer();
    while (root.endsWith('/'))
        root.chop(1);
    return root + "/authserver";
}

QString YggdrasilAuthStep::describe()
{
    return m_refresh ? tr("Refreshing %1 session").arg(serviceName()) : tr("Logging in to %1").arg(serviceName());
}

void YggdrasilAuthStep::perform()
{
    auto clientToken = m_data->yggdrasilToken.extra["clientToken"].toString();
    if (clientToken.isEmpty()) {
        clientToken = QUuid::createUuid().toString(QUuid::Id128);
        m_data->yggdrasilToken.extra["clientToken"] = clientToken;
    }

    QJsonObject body;
    QUrl url;
    if (m_refresh) {
        if (m_data->yggdrasilToken.token.isEmpty()) {
            emit finished(AccountTaskState::STATE_FAILED_HARD, tr("No session is stored. Please log in again."));
            return;
        }
        url = QUrl(authserverUrl() + "/refresh");
        body["accessToken"] = m_data->yggdrasilToken.token;
    } else {
        url = QUrl(authserverUrl() + "/authenticate");
        body["username"] = m_data->elyLogin;
        body["password"] = m_data->elyPassword;
        QJsonObject agent;
        agent["name"] = "Minecraft";
        agent["version"] = 1;
        body["agent"] = agent;
    }
    body["clientToken"] = clientToken;
    body["requestUser"] = true;

    auto headers = QList<Net::HeaderPair>{
        { "Content-Type", "application/json" },
        { "Accept", "application/json" },
    };

    auto [request, response] = Net::Request::makeByteArray(url, QJsonDocument(body).toJson(QJsonDocument::Compact));
    m_request = request;
    m_request->addHeaderProxy(std::make_unique<Net::RawHeaderProxy>(headers));

    m_task.reset(new NetJob("YggdrasilAuthStep", APPLICATION->network()));
    m_task->setAskRetry(false);
    m_task->addNetAction(m_request);

    connect(m_task.get(), &Task::finished, this, [this, response] { onRequestDone(response); });

    m_task->start();
}

void YggdrasilAuthStep::abort()
{
    if (m_task)
        m_task->abort();
}

void YggdrasilAuthStep::onRequestDone(QByteArray* response)
{
    // never keep the password in memory longer than needed
    m_data->elyPassword.clear();

    auto obj = QJsonDocument::fromJson(*response).object();

    if (m_request->error() != QNetworkReply::NoError) {
        qWarning() << "Yggdrasil reply error:" << m_request->error();
        QString message = obj.value("errorMessage").toString();
        if (message.isEmpty())
            message = m_request->errorString();
        if (message.contains("two factor", Qt::CaseInsensitive)) {
            message = tr("This account is protected with two-factor authentication. Enter the code from your authenticator app.");
        }
        if (Net::isApplicationError(m_request->error()) && !Net::isServerError(m_request->error())) {
            emit finished(m_refresh ? AccountTaskState::STATE_FAILED_HARD : AccountTaskState::STATE_FAILED_SOFT,
                          QString("%1: %2").arg(serviceName(), message));
        } else {
            m_data->networkError = m_request->error();
            emit finished(AccountTaskState::STATE_OFFLINE, QString("%1: %2").arg(serviceName(), message));
        }
        return;
    }

    auto accessToken = obj.value("accessToken").toString();
    auto profile = obj.value("selectedProfile").toObject();
    if (profile.isEmpty()) {
        // some servers do not select a profile automatically, take the first one
        auto profiles = obj.value("availableProfiles").toArray();
        if (!profiles.isEmpty())
            profile = profiles.first().toObject();
    }
    if (accessToken.isEmpty()) {
        emit finished(AccountTaskState::STATE_FAILED_SOFT, tr("%1 returned an unexpected response.").arg(serviceName()));
        return;
    }
    if (profile.value("id").toString().isEmpty()) {
        emit finished(AccountTaskState::STATE_FAILED_SOFT,
                      tr("This account has no Minecraft profile (character) on %1.").arg(serviceName()));
        return;
    }

    m_data->yggdrasilToken.token = accessToken;
    m_data->yggdrasilToken.validity = Validity::Certain;
    m_data->yggdrasilToken.issueInstant = QDateTime::currentDateTimeUtc();
    m_data->yggdrasilToken.notAfter = QDateTime();
    auto clientToken = obj.value("clientToken").toString();
    if (!clientToken.isEmpty())
        m_data->yggdrasilToken.extra["clientToken"] = clientToken;

    auto name = profile.value("name").toString();
    m_data->minecraftProfile.id = profile.value("id").toString().remove('-');
    m_data->minecraftProfile.name = name;
    m_data->minecraftProfile.validity = Validity::Certain;
    m_data->yggdrasilToken.extra["userName"] = name;

    if (m_data->type == AccountType::ElyBy) {
        // Ely.by skin system redirects to the actual skin texture of the player
        m_data->minecraftProfile.skin.id = "ely";
        m_data->minecraftProfile.skin.url = QString("https://skinsystem.ely.by/skins/%1.png").arg(name);
        m_data->minecraftProfile.skin.variant = "CLASSIC";
    }

    m_data->minecraftEntitlement.ownsMinecraft = true;
    m_data->minecraftEntitlement.canPlayMinecraft = true;
    m_data->minecraftEntitlement.validity = Validity::Certain;

    emit finished(AccountTaskState::STATE_WORKING, tr("Logged in to %1").arg(serviceName()));
}
