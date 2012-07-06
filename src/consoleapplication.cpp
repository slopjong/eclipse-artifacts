#include <QDebug>

#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include <QtXmlPatterns/QXmlNodeModelIndex>
#include <QtXmlPatterns/QSimpleXmlNodeModel>
#include <QtXmlPatterns/QXmlQuery>
#include <QtXmlPatterns/QXmlResultItems>

#include <cstdlib>
#include <typeinfo>

#include "quazip.h"
#include "quazipfile.h"

#include "consoleapplication.h"

ConsoleApplication::ConsoleApplication(int argc, char *argv[]) :
    QCoreApplication(argc, argv)
{
}

void ConsoleApplication::process()
{
    // coming from the console later
    QString url = "http://appwrench.onpositive.com/static/updatesite/";

    // TODO: check if the trailing slash is there or not
    /* some code */

    m_updateSite = url;

    url.append("site.xml");

    connect(&m_downloader, SIGNAL(downloadFinished(QBuffer*)), SLOT(slotUpdatesiteDownloadFinished(QBuffer*)));
    m_downloader.get(url);


    /*
    QTextStream cout(stdout, QIODevice::WriteOnly);

    QuaZip zipFile("resources/com.onpositive.gae.appwrench_1.5.0.jar");
    zipFile.open(QuaZip::mdUnzip);
    QuaZipFileInfo info;
    QuaZipFile aFile(&zipFile);

    bool more = zipFile.goToFirstFile();

    if(!more)
    {
        cout << "The jar file doesn't contain any files." << endl;
        exit(1);
    }

    while(more)
    {
        zipFile.getCurrentFileInfo(&info);
        aFile.open(QIODevice::ReadOnly);
        QString name = aFile.getActualFileName();

        if(name == "feature.xml")
            break;

        more = zipFile.goToNextFile();
        aFile.close();
    }

    QByteArray xmlContent = aFile.readAll();
    aFile.close();
    QBuffer sourceDocument(&xmlContent);
    sourceDocument.open(QIODevice::ReadOnly);

    QXmlQuery query(QXmlQuery::XPath20);
    query.bindVariable("inputDocument", &sourceDocument);
    query.setQuery("doc($inputDocument)/feature/plugin");

    if (!query.isValid())
    {
        cout << "query valid" << endl;
        exit(1);
    }

    QXmlResultItems results;
    query.evaluateTo(&results);

    QXmlItem item(results.next());
    while(!item.isNull())
    {
        if (item.isNull())
        {
            cout << "query is null" << endl;
            exit(1);
        }
        else if( item.isNode())
        {
            cout << "item is node" << endl;
        }
        else if( item.isAtomicValue())
        {
            cout << "item is atomic" << endl;
        }

        QXmlQuery tmpQuery;

        tmpQuery.bindVariable("featureNode", item);
        tmpQuery.setQuery("$featureNode/@id/string()");

        if (!tmpQuery.isValid())
        {
            cout << "tmpquery not valid" << endl;
            exit(1);
        }

        QStringList tmpResults;
        if(tmpQuery.evaluateTo(&tmpResults))
            cout << tmpResults.first() << endl;
        else
            cout << "Could not evaluate the query";

        item = results.next();
    }

    sourceDocument.close();

    exit(0);
    */
}

void ConsoleApplication::slotUpdatesiteDownloadFinished(QBuffer *siteXml)
{
    qDebug() << "update";

    m_downloader.disconnect();
    connect(&m_downloader, SIGNAL(downloadFinished(QBuffer*)), SLOT(slotFeatureDownloadFinished(QBuffer*)));

    siteXml->open(QIODevice::ReadOnly);

    //QXmlQuery query(QXmlQuery::XPath20);
    QXmlQuery query;
    query.bindVariable("inputDocument", siteXml);
    query.setQuery("doc($inputDocument)//feature");

    if (!query.isValid())
    {
        qDebug() << "Query invalid.";
        return;
    }

    QXmlResultItems results;
    query.evaluateTo(&results);

    QXmlItem item(results.next());
    while(!item.isNull())
    {
        QXmlQuery tmpQuery;

        tmpQuery.bindVariable("featureNode", item);

        // for whatever reason the query doesn't work
        // with attribute names other than id, version
        // and name and thus * is used.
        tmpQuery.setQuery("$featureNode/@*/string()");

        if (!tmpQuery.isValid())
        {
            qDebug() << "Sub-query invalid.";
            return;
        }

        QStringList tmpResults;
        if(!tmpQuery.evaluateTo(&tmpResults))
        {
            qDebug() << "Could not evaluate the sub-query.";
            return;
        }

        // because we fetch all the attributes we must filter
        // the one containing .jar, a nasty workaround though
        foreach(QString el, tmpResults)
        {
            if(el.contains(".jar"))
                m_features << el;
        }

        item = results.next();
    }

    foreach(QString feature, m_features)
    {
        qDebug() << "Getting file " << QString("%1%2").arg(m_updateSite).arg(feature);
        //m_downloader.get(QString("%1%2").arg(m_updateSite).arg(feature));
        m_downloader.get("http://appwrench.onpositive.com/static/updatesite/features/com.onpositive.gae.appwrench_1.5.0.jar");
    }

    siteXml->close();
    delete siteXml;
}

void ConsoleApplication::slotFeatureDownloadFinished(QBuffer *data)
{
    data->open(QIODevice::ReadOnly);
    QByteArray ba = data->readAll();
    data->close();
    delete data;

    qDebug() << "Feature downloaded";
    //qDebug() << QString("Download finished:\n=================\n%1").arg(ba.constData());
}


void ConsoleApplication::slotPluginDownloadFinished(QBuffer *data)
{
    data->open(QIODevice::ReadOnly);
    QByteArray ba = data->readAll();
    data->close();
    delete data;

    qDebug() << QString("Download finished:\n=================\n%1").arg(ba.constData());
}
