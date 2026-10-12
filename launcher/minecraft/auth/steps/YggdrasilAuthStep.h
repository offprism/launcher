#pragma once
#include <QObject>

#include "minecraft/auth/AuthStep.h"
#include "net/NetJob.h"
#include "net/Request.h"

// Authenticates against a Yggdrasil-compatible auth server: Ely.by or any authlib-injector server.
// If refresh is true, the stored access token is refreshed, otherwise login/password (from AccountData) are used.
class YggdrasilAuthStep : public AuthStep {
    Q_OBJECT

   public:
    explicit YggdrasilAuthStep(AccountData* data, bool refresh);
    virtual ~YggdrasilAuthStep() noexcept = default;

    void perform() override;
    void abort() override;

    QString describe() override;

   private slots:
    void onRequestDone(QByteArray* response);

   private:
    QString serviceName() const;
    QString authserverUrl() const;

    bool m_refresh;
    Net::Request::Ptr m_request;
    NetJob::Ptr m_task;
};
