#include "ElyByStep.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QUuid>

#include "Application.h"
#include "Logging.h"
#include "net/NetUtils.h"
#include "net/RawHeaderProxy.h"
#include "net/Request.h"

ElyByStep::ElyByStep(AccountData* data, bool refresh) : AuthStep(data), m_refresh(refresh) {}

QString ElyByStep::describe()
{
    return m_refresh ? tr("Refreshing Ely.by session") : tr("Logging in to Ely.by");
}

void ElyByStep::perform()
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
            emit finished(AccountTaskState::STATE_FAILED_HARD, tr("No Ely.by session is stored. Please log in again."));
            return;
        }
        url = QUrl("https://authserver.ely.by/auth/refresh");
        body["accessToken"] = m_data->yggdrasilToken.token;
    } else {
        url = QUrl("https://authserver.ely.by/auth/authenticate");
        body["username"] = m_data->elyLogin;
        body["password"] = m_data->elyPassword;
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

    m_task.reset(new NetJob("ElyByStep", APPLICATION->network()));
    m_task->setAskRetry(false);
    m_task->addNetAction(m_request);

    connect(m_task.get(), &Task::finished, this, [this, response] { onRequestDone(response); });

    m_task->start();
}

void ElyByStep::abort()
{
    if (m_task)
        m_task->abort();
}

void ElyByStep::onRequestDone(QByteArray* response)
{
    // never keep the password in memory longer than needed
    m_data->elyPassword.clear();

    auto doc = QJsonDocument::fromJson(*response);
    auto obj = doc.object();

    if (m_request->error() != QNetworkReply::NoError) {
        qWarning() << "Ely.by reply error:" << m_request->error();
        QString message = obj.value("errorMessage").toString();
        if (message.isEmpty())
            message = m_request->errorString();
        if (message.contains("two factor", Qt::CaseInsensitive)) {
            message = tr("This account is protected with two-factor authentication. Enter the code from your authenticator app.");
        }
        if (Net::isApplicationError(m_request->error()) && !Net::isServerError(m_request->error())) {
            emit finished(m_refresh ? AccountTaskState::STATE_FAILED_HARD : AccountTaskState::STATE_FAILED_SOFT,
                          tr("Ely.by: %1").arg(message));
        } else {
            m_data->networkError = m_request->error();
            emit finished(AccountTaskState::STATE_OFFLINE, tr("Ely.by: %1").arg(message));
        }
        return;
    }

    auto accessToken = obj.value("accessToken").toString();
    auto profile = obj.value("selectedProfile").toObject();
    if (accessToken.isEmpty() || profile.value("id").toString().isEmpty()) {
        emit finished(AccountTaskState::STATE_FAILED_SOFT, tr("Ely.by returned an unexpected response."));
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

    // Ely.by skin system redirects to the actual skin texture of the player
    m_data->minecraftProfile.skin.id = "ely";
    m_data->minecraftProfile.skin.url = QString("https://skinsystem.ely.by/skins/%1.png").arg(name);
    m_data->minecraftProfile.skin.variant = "CLASSIC";

    m_data->minecraftEntitlement.ownsMinecraft = true;
    m_data->minecraftEntitlement.canPlayMinecraft = true;
    m_data->minecraftEntitlement.validity = Validity::Certain;

    emit finished(AccountTaskState::STATE_WORKING, tr("Logged in to Ely.by"));
}
