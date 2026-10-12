#include "ElyByLoginDialog.h"

#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

ElyByLoginDialog::ElyByLoginDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(tr("Add Ely.by account"));
    setMinimumWidth(420);

    auto* layout = new QVBoxLayout(this);

    auto* header = new QLabel(tr("Log in with your <a href=\"https://ely.by\">Ely.by</a> account. "
                                 "Your skin from Ely.by will be shown in the launcher and in game."),
                              this);
    header->setWordWrap(true);
    header->setOpenExternalLinks(true);
    layout->addWidget(header);

    auto* form = new QFormLayout();
    m_login = new QLineEdit(this);
    m_login->setPlaceholderText(tr("Nickname or e-mail"));
    m_password = new QLineEdit(this);
    m_password->setEchoMode(QLineEdit::Password);
    m_totp = new QLineEdit(this);
    m_totp->setPlaceholderText(tr("Only if two-factor authentication is enabled"));
    m_totp->setMaxLength(8);
    form->addRow(tr("Login:"), m_login);
    form->addRow(tr("Password:"), m_password);
    form->addRow(tr("2FA code:"), m_totp);
    layout->addLayout(form);

    auto* registerLabel = new QLabel(tr("No account yet? <a href=\"https://account.ely.by/register\">Register on Ely.by</a>"), this);
    registerLabel->setOpenExternalLinks(true);
    layout->addWidget(registerLabel);

    m_status = new QLabel(this);
    m_status->setWordWrap(true);
    m_status->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout->addWidget(m_status);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_buttons->button(QDialogButtonBox::Ok)->setText(tr("Log in"));
    layout->addWidget(m_buttons);

    connect(m_buttons, &QDialogButtonBox::accepted, this, &ElyByLoginDialog::onLogin);
    connect(m_buttons, &QDialogButtonBox::rejected, this, [this] {
        if (m_task && m_task->isRunning())
            m_task->abort();
        reject();
    });
}

void ElyByLoginDialog::setBusy(bool busy)
{
    m_login->setEnabled(!busy);
    m_password->setEnabled(!busy);
    m_totp->setEnabled(!busy);
    m_buttons->button(QDialogButtonBox::Ok)->setEnabled(!busy);
}

void ElyByLoginDialog::onLogin()
{
    auto login = m_login->text().trimmed();
    auto password = m_password->text();
    if (login.isEmpty() || password.isEmpty()) {
        m_status->setText(tr("Please enter your login and password."));
        return;
    }
    auto totp = m_totp->text().trimmed();
    if (!totp.isEmpty()) {
        // Ely.by expects the TOTP code appended to the password
        password += ":" + totp;
    }

    setBusy(true);
    m_status->setText(tr("Logging in..."));

    m_account = MinecraftAccount::createElyBy(login, password);
    m_task = m_account->login();
    connect(m_task.get(), &Task::failed, this, &ElyByLoginDialog::onTaskFailed);
    connect(m_task.get(), &Task::succeeded, this, &ElyByLoginDialog::onTaskSucceeded);
    connect(m_task.get(), &Task::status, this, [this](QString status) { m_status->setText(status); });
    m_task->start();
}

void ElyByLoginDialog::onTaskFailed(const QString& reason)
{
    setBusy(false);
    m_status->setText(QString("<b>%1</b>").arg(reason.toHtmlEscaped()));
}

void ElyByLoginDialog::onTaskSucceeded()
{
    accept();
}

MinecraftAccountPtr ElyByLoginDialog::newAccount(QWidget* parent)
{
    ElyByLoginDialog dlg(parent);
    if (dlg.exec() == QDialog::Accepted) {
        return dlg.m_account;
    }
    return nullptr;
}
