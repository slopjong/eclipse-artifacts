
#include <QDebug>

#include <QtCore/QByteArray>
#include <QtCore/QFileInfo>

#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent) :
    QNetworkAccessManager(parent)
{
    connect(this, SIGNAL(finished(QNetworkReply *)),
            SLOT(slotFinished(QNetworkReply *)));
}

void DownloadManager::get(QUrl url)
{
    QNetworkRequest request(url);
    QNetworkAccessManager::get(request);

    QString fileName = QFileInfo(url.path()).fileName();

    // this signal doesn't make much sense, the requests are queued
    // and thus it can't be used to display a progress
    //emit downloadingFile(fileName);
}

/**
 * This slot emits the downloadFinished signal and transmits a QBuffer which is
 * a QIODevice. The user has the responsibility to delete the buffer.
 *
 * @brief DownloadManager::slotFinished
 * @param reply
 */
void DownloadManager::slotFinished(QNetworkReply *reply)
{
    QByteArray ba = reply->readAll();
    QBuffer * buffer = new QBuffer(&ba);

    // get the file name
    QUrl url = reply->url();
    QString fileName = QFileInfo(url.path()).fileName();

    reply->deleteLater();

    emit downloadFinished(buffer, fileName);
}
