
#include <QDebug>

#include <QtCore/QByteArray>

#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent) :
    QNetworkAccessManager(parent)
{
}

void DownloadManager::get(QString url)
{
    QNetworkRequest request(url);
    QNetworkAccessManager::get(request);

    connect(this, SIGNAL(finished(QNetworkReply *)), SLOT(slotFinished(QNetworkReply *)));
}

/**
 * This slot emits the downloadFinished signal and transmits a QBuffer which is a
 * QIODevice. The user has the responsibility to delete the buffer.
 *
 * @brief DownloadManager::slotFinished
 * @param reply
 */
void DownloadManager::slotFinished(QNetworkReply *reply)
{
    QByteArray ba = reply->readAll();
    QBuffer * buffer = new QBuffer(&ba);

    reply->deleteLater();

    emit downloadFinished(buffer);
}
