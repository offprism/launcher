#pragma once
#include <QObject>

#include "minecraft/auth/AuthStep.h"
#include "net/NetJob.h"
#include "net/Request.h"

// Reads the skin of the profile from the session server of an authlib-injector server.
class AuthlibProfileStep : public AuthStep {
    Q_OBJECT

   public:
    explicit AuthlibProfileStep(AccountData* data);
    virtual ~AuthlibProfileStep() noexcept = default;

    void perform() override;

    QString describe() override;

   private slots:
    void onRequestDone(QByteArray* response);

   private:
    Net::Request::Ptr m_request;
    NetJob::Ptr m_task;
};
