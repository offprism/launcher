#pragma once

#include <QDialog>

#include "minecraft/auth/MinecraftAccount.h"

class QLineEdit;
class QLabel;
class QDialogButtonBox;
class QNetworkReply;

// Login dialog for Yggdrasil-compatible accounts: Ely.by and any authlib-injector server.
class ElyByLoginDialog : public QDialog {
    Q_OBJECT

   public:
    enum class Mode { ElyBy, AuthlibInjector };

    explicit ElyByLoginDialog(Mode mode, QWidget* parent = nullptr);
    ~ElyByLoginDialog() override = default;

    static MinecraftAccountPtr newAccount(QWidget* parent);
    static MinecraftAccountPtr newAuthlibAccount(QWidget* parent, const QString& server = {});

   private slots:
    void onLogin();
    void onTaskFailed(const QString& reason);
    void onTaskSucceeded();

   private:
    void setBusy(bool busy);
    void resolveServer(const QString& input);
    void startLogin(const QString& server);
    void fail(const QString& reason);

    Mode m_mode;
    QLineEdit* m_server = nullptr;
    QLineEdit* m_login = nullptr;
    QLineEdit* m_password = nullptr;
    QLineEdit* m_totp = nullptr;
    QLabel* m_status = nullptr;
    QDialogButtonBox* m_buttons = nullptr;

    MinecraftAccountPtr m_account;
    shared_qobject_ptr<AuthFlow> m_task;
    QNetworkReply* m_resolveReply = nullptr;
};
