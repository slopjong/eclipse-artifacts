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

signals:
    
public slots:

private slots:

    void slotDownloadFinished(QBuffer *data);
    
};

#endif // CONSOLEAPPLICATION_H
