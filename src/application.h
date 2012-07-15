#ifndef CONSOLEAPPLICATION_H
#define CONSOLEAPPLICATION_H

#include <QtGui/QApplication>

#include <QtCore/QBuffer>
#include <QtCore/QStringList>
#include <QtCore/QHash>

#include <QtXmlPatterns/QXmlQuery>

#include <atomic>

#include "downloadmanager.h"
#include "mainwindow.h"

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int argc, char *argv[]);
    void process();
    
private:

    MainWindow *m_mainwindow;

    bool m_gui_mode;

    DownloadManager m_site_downloader;
    DownloadManager m_feature_downloader;
    DownloadManager m_plugin_downloader;

    QXmlQuery::QueryLanguage m_queryLanguage;

    QStringList m_features;
    QStringList m_plugins;

    QHash<QString, QString> md5_hashes;
    QHash<QString, QString> sha1_hashes;

    QHash<QString, QString> m_pkgbuild_variables;

    std::atomic<int> m_amount_features;
    std::atomic<int> m_amount_plugins;

    std::atomic<int> m_amount_processed_features;
    std::atomic<int> m_amount_processed_plugins;

    QByteArray getFileFromZip(QString file, QBuffer *zip);
    void calculateHashes(QString file, QByteArray & data);
    bool downloadsFinished();
    QStringList variableTemplates();
    void initVariables();

signals:

    void createPKGBUILD();
    
public slots:

private slots:

    void slotUpdatesiteDownloadFinished(QBuffer *data, QString fileName);
    void slotFeatureDownloadFinished(QBuffer *data, QString fileName);
    void slotPluginDownloadFinished(QBuffer *data, QString fileName);
    void slotDownloadsFinished();
    
};

#endif // CONSOLEAPPLICATION_H
