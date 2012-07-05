#ifndef CONSOLEAPPLICATION_H
#define CONSOLEAPPLICATION_H

#include <QCoreApplication>

#include <QtCore/QBuffer>

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

signals:
    
public slots:

private slots:

    void slotUpdatesiteDownloadFinished(QBuffer *data);
    void slotFeatureDownloadFinished(QBuffer *data);
    void slotPluginDownloadFinished(QBuffer *data);
    
};

#endif // CONSOLEAPPLICATION_H
