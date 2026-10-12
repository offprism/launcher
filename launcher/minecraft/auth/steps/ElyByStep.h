#pragma once
#include <QObject>

#include "minecraft/auth/AuthStep.h"
#include "net/NetJob.h"
#include "net/Request.h"

// Authenticates against the Ely.by Yggdrasil-compatible auth server.
// If refresh is true, the stored access token is refreshed, otherwise username/password (from AccountData) are used.
class ElyByStep : public AuthStep {
    Q_OBJECT

   public:
    explicit ElyByStep(AccountData* data, bool refresh);
    virtual ~ElyByStep() noexcept = default;

    void perform() override;
    void abort() override;

    QString describe() override;

   private slots:
    void onRequestDone(QByteArray* response);

   private:
    bool m_refresh;
    Net::Request::Ptr m_request;
    NetJob::Ptr m_task;
};
