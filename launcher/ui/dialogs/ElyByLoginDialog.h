#pragma once

#include <QDialog>

#include "minecraft/auth/MinecraftAccount.h"

class QLineEdit;
class QLabel;
class QDialogButtonBox;

class ElyByLoginDialog : public QDialog {
    Q_OBJECT

   public:
    explicit ElyByLoginDialog(QWidget* parent = nullptr);
    ~ElyByLoginDialog() override = default;

    static MinecraftAccountPtr newAccount(QWidget* parent);

   private slots:
    void onLogin();
    void onTaskFailed(const QString& reason);
    void onTaskSucceeded();

   private:
    void setBusy(bool busy);

    QLineEdit* m_login = nullptr;
    QLineEdit* m_password = nullptr;
    QLineEdit* m_totp = nullptr;
    QLabel* m_status = nullptr;
    QDialogButtonBox* m_buttons = nullptr;

    MinecraftAccountPtr m_account;
    shared_qobject_ptr<AuthFlow> m_task;
};
