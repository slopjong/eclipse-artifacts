#ifndef CONSOLEAPPLICATION_H
#define CONSOLEAPPLICATION_H

#include <QtGui/QApplication>

#include <QtCore/QBuffer>
#include <QtCore/QStringList>
#include <QtCore/QHash>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include <QtXmlPatterns/QXmlQuery>

#include <atomic>

#include "downloadmanager.h"
#include "mainwindow.h"

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int argc, char *argv[]);
    ~Application();
    void process();
    
private:

    MainWindow *m_gui;

    bool m_gui_mode;

    DownloadManager m_site_downloader;
    DownloadManager m_feature_downloader;
    DownloadManager m_plugin_downloader;
    QNetworkAccessManager m_head_request;

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
    QString sanitizeUpdatesite(QString updateSite);

signals:

    void createPKGBUILD();
    void updatesiteValid();
    void updatesiteInvalid();
    void updatesiteLoading();
    
public slots:

private slots:

    void slotUpdatesiteDownloadFinished(QBuffer *data, QString fileName);
    void slotFeatureDownloadFinished(QBuffer *data, QString fileName);
    void slotPluginDownloadFinished(QBuffer *data, QString fileName);
    void slotCreatePkgbuild();
    void slotUpdatesiteChanged(QString updateSite);
    void slotHeadRequestFinished(QNetworkReply *reply);
    
};

#endif // CONSOLEAPPLICATION_H
