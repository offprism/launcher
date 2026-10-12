#include "ElyByLoginDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

#include "Application.h"

ElyByLoginDialog::ElyByLoginDialog(Mode mode, QWidget* parent) : QDialog(parent), m_mode(mode)
{
    setMinimumWidth(440);

    auto* layout = new QVBoxLayout(this);

    auto* header = new QLabel(this);
    header->setWordWrap(true);
    header->setOpenExternalLinks(true);
    layout->addWidget(header);

    auto* form = new QFormLayout();
    if (m_mode == Mode::AuthlibInjector) {
        setWindowTitle(tr("Add authlib-injector account"));
        header->setText(tr("Log in to any <a href=\"https://github.com/yushijinhun/authlib-injector\">authlib-injector</a> "
                           "compatible skin server (for example LittleSkin or a Blessing Skin server). "
                           "The skin from that server will be shown in the launcher and in game."));
        m_server = new QLineEdit(this);
        m_server->setPlaceholderText("https://littleskin.cn/api/yggdrasil");
        form->addRow(tr("Server:"), m_server);
    } else {
        setWindowTitle(tr("Add Ely.by account"));
        header->setText(tr("Log in with your <a href=\"https://ely.by\">Ely.by</a> account. "
                           "Your skin from Ely.by will be shown in the launcher and in game."));
    }

    m_login = new QLineEdit(this);
    m_login->setPlaceholderText(m_mode == Mode::ElyBy ? tr("Nickname or e-mail") : tr("E-mail or nickname"));
    m_password = new QLineEdit(this);
    m_password->setEchoMode(QLineEdit::Password);
    form->addRow(tr("Login:"), m_login);
    form->addRow(tr("Password:"), m_password);
    if (m_mode == Mode::ElyBy) {
        m_totp = new QLineEdit(this);
        m_totp->setPlaceholderText(tr("Only if two-factor authentication is enabled"));
        m_totp->setMaxLength(8);
        form->addRow(tr("2FA code:"), m_totp);
    }
    layout->addLayout(form);

    if (m_mode == Mode::ElyBy) {
        auto* registerLabel = new QLabel(tr("No account yet? <a href=\"https://account.ely.by/register\">Register on Ely.by</a>"), this);
        registerLabel->setOpenExternalLinks(true);
        layout->addWidget(registerLabel);
    }

    m_status = new QLabel(this);
    m_status->setWordWrap(true);
    m_status->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout->addWidget(m_status);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_buttons->button(QDialogButtonBox::Ok)->setText(tr("Log in"));
    layout->addWidget(m_buttons);

    connect(m_buttons, &QDialogButtonBox::accepted, this, &ElyByLoginDialog::onLogin);
    connect(m_buttons, &QDialogButtonBox::rejected, this, [this] {
        if (m_resolveReply)
            m_resolveReply->abort();
        if (m_task && m_task->isRunning())
            m_task->abort();
        reject();
    });
}

void ElyByLoginDialog::setBusy(bool busy)
{
    if (m_server)
        m_server->setEnabled(!busy);
    m_login->setEnabled(!busy);
    m_password->setEnabled(!busy);
    if (m_totp)
        m_totp->setEnabled(!busy);
    m_buttons->button(QDialogButtonBox::Ok)->setEnabled(!busy);
}

void ElyByLoginDialog::fail(const QString& reason)
{
    setBusy(false);
    m_status->setText(QString("<b>%1</b>").arg(reason.toHtmlEscaped()));
}

void ElyByLoginDialog::onLogin()
{
    if (m_login->text().trimmed().isEmpty() || m_password->text().isEmpty()) {
        m_status->setText(tr("Please enter your login and password."));
        return;
    }
    if (m_mode == Mode::ElyBy) {
        startLogin({});
        return;
    }

    auto server = m_server->text().trimmed();
    if (server.isEmpty()) {
        m_status->setText(tr("Please enter the address of the authlib-injector server."));
        return;
    }
    if (!server.contains("://"))
        server.prepend("https://");
    resolveServer(server);
}

// authlib-injector API Location Indication: the server may point to the real API root with a header
void ElyByLoginDialog::resolveServer(const QString& input)
{
    setBusy(true);
    m_status->setText(tr("Contacting the server..."));

    QNetworkRequest request{ QUrl(input) };
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    m_resolveReply = APPLICATION->network()->get(request);
    connect(m_resolveReply, &QNetworkReply::finished, this, [this, input] {
        auto* reply = m_resolveReply;
        m_resolveReply = nullptr;
        reply->deleteLater();

        if (reply->error() == QNetworkReply::OperationCanceledError)
            return;

        QString root = input;
        auto location = QString::fromUtf8(reply->rawHeader("X-Authlib-Injector-API-Location"));
        if (!location.isEmpty()) {
            root = reply->url().resolved(QUrl(location)).toString();
        } else if (reply->error() != QNetworkReply::NoError) {
            fail(tr("Could not reach the server: %1").arg(reply->errorString()));
            return;
        } else {
            auto meta = QJsonDocument::fromJson(reply->readAll()).object();
            if (!meta.contains("meta") && !meta.contains("skinDomains")) {
                fail(tr("This address is not an authlib-injector server."));
                return;
            }
        }
        while (root.endsWith('/'))
            root.chop(1);
        startLogin(root);
    });
}

void ElyByLoginDialog::startLogin(const QString& server)
{
    auto login = m_login->text().trimmed();
    auto password = m_password->text();
    if (m_totp && !m_totp->text().trimmed().isEmpty()) {
        // Ely.by expects the TOTP code appended to the password
        password += ":" + m_totp->text().trimmed();
    }

    setBusy(true);
    m_status->setText(tr("Logging in..."));

    m_account = m_mode == Mode::ElyBy ? MinecraftAccount::createElyBy(login, password)
                                      : MinecraftAccount::createAuthlibInjector(server, login, password);
    m_task = m_account->login();
    connect(m_task.get(), &Task::failed, this, &ElyByLoginDialog::onTaskFailed);
    connect(m_task.get(), &Task::succeeded, this, &ElyByLoginDialog::onTaskSucceeded);
    connect(m_task.get(), &Task::status, this, [this](QString status) { m_status->setText(status); });
    m_task->start();
}

void ElyByLoginDialog::onTaskFailed(const QString& reason)
{
    fail(reason);
}

void ElyByLoginDialog::onTaskSucceeded()
{
    accept();
}

MinecraftAccountPtr ElyByLoginDialog::newAccount(QWidget* parent)
{
    ElyByLoginDialog dlg(Mode::ElyBy, parent);
    if (dlg.exec() == QDialog::Accepted) {
        return dlg.m_account;
    }
    return nullptr;
}

MinecraftAccountPtr ElyByLoginDialog::newAuthlibAccount(QWidget* parent, const QString& server)
{
    ElyByLoginDialog dlg(Mode::AuthlibInjector, parent);
    dlg.m_server->setText(server);
    if (dlg.exec() == QDialog::Accepted) {
        return dlg.m_account;
    }
    return nullptr;
}
