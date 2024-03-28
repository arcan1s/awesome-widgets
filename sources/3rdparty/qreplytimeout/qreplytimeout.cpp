#include "qreplytimeout.h"

#include <QTimer>


QReplyTimeout::QReplyTimeout(QNetworkReply *reply, const int timeout)
    : QObject(reply)
{
    QTimer::singleShot(timeout, this, SLOT(timeout()));
}


void QReplyTimeout::timeout()
{
    auto reply = dynamic_cast<QNetworkReply *>(parent());
    if (reply->isRunning())
        reply->close();
}
