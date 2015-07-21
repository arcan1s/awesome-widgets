// from here http://codereview.stackexchange.com/questions/30031/qnetworkreply-network-reply-timeout-helper
// no license provided

#include <QNetworkReply>
#include <QObject>


class QReplyTimeout : public QObject
{
    Q_OBJECT

public:
    explicit QReplyTimeout(QNetworkReply *reply, const int timeout);

public slots:
    void timeout();
};
