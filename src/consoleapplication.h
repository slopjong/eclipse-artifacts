#ifndef CONSOLEAPPLICATION_H
#define CONSOLEAPPLICATION_H

#include <QCoreApplication>

#include <QtCore/QBuffer>
#include <QtCore/QStringList>

#include <atomic>

#include "downloadmanager.h"

class ConsoleApplication : public QCoreApplication
{
    Q_OBJECT

public:
    ConsoleApplication(int argc, char *argv[]);
    void process();
    
private:

    DownloadManager m_downloader;
    QString m_updateSite;
    QStringList m_features;

    std::atomic<int> m_amount_features;
    std::atomic<int> m_amount_plugins;

    QByteArray * getFileFromZip(QString file, QBuffer *zip);

signals:
    
public slots:

private slots:

    void slotUpdatesiteDownloadFinished(QBuffer *data);
    void slotFeatureDownloadFinished(QBuffer *data);
    void slotPluginDownloadFinished(QBuffer *data);
    
};

#endif // CONSOLEAPPLICATION_H
