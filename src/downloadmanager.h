#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>

#include <QtCore/QByteArray>
#include <QtCore/QBuffer>

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

class DownloadManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit DownloadManager(QObject *parent = 0);
    void get(QUrl url);

signals:

    // this signal doesn't make much sense, the requests are queued
    void downloadingFile(QString file);
    void downloadFinished(QBuffer * buffer, QString fileName);

public slots:

private slots:

    void slotFinished(QNetworkReply *reply);
};

#endif // DOWNLOADMANAGER_H
