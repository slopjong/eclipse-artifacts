#ifndef CONSOLEAPPLICATION_H
#define CONSOLEAPPLICATION_H

#include <QCoreApplication>

#include <QtCore/QBuffer>
#include <QtCore/QStringList>
#include <QtCore/QHash>

#include <QtXmlPatterns/QXmlQuery>

#include <atomic>

#include "downloadmanager.h"

class ConsoleApplication : public QCoreApplication
{
    Q_OBJECT

public:
    ConsoleApplication(int argc, char *argv[]);
    void process();
    
private:

    DownloadManager m_site_downloader;
    DownloadManager m_feature_downloader;
    DownloadManager m_plugin_downloader;

    QXmlQuery::QueryLanguage m_queryLanguage;

    QString m_updateSite;
    QStringList m_features;
    QStringList m_plugins;

    QHash<QString, QString> md5_hashes;
    QHash<QString, QString> sha1_hashes;

    std::atomic<int> m_amount_features;
    std::atomic<int> m_amount_plugins;

    QByteArray * getFileFromZip(QString file, QBuffer *zip);
    void calculateHashes(QString file, QByteArray & data);

signals:
    
public slots:

private slots:

    void slotUpdatesiteDownloadFinished(QBuffer *data, QString fileName);
    void slotFeatureDownloadFinished(QBuffer *data, QString fileName);
    void slotPluginDownloadFinished(QBuffer *data, QString fileName);
    
};

#endif // CONSOLEAPPLICATION_H
