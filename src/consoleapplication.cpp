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
    QCoreApplication(argc, argv),
    m_amount_features(0),
    m_amount_plugins(0)
{
}

void ConsoleApplication::process()
{
    // coming from the console later
    QString url = "http://appwrench.onpositive.com/static/updatesite/";
    //QString url = "http://download.eclipse.org/tools/cdt/releases/helios/";

    // TODO: check if the trailing slash is there or not
    /* some code */

    m_updateSite = url;

    url.append("site.xml");

    connect(&m_downloader, SIGNAL(downloadFinished(QBuffer*)), SLOT(slotUpdatesiteDownloadFinished(QBuffer*)));
    m_downloader.get(url);
}

void ConsoleApplication::slotUpdatesiteDownloadFinished(QBuffer *siteXml)
{
    qDebug() << "site.xml downloaded from the update site.";

    // disconnect this slot because only one site.xml
    // is downloaded for one eclipse plugin
    m_downloader.disconnect(this);

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

    m_amount_features.fetch_add(m_features.size());

    // connect the 'feature downloaded' slot
    connect(&m_downloader, SIGNAL(downloadFinished(QBuffer*)), this, SLOT(slotFeatureDownloadFinished(QBuffer*)));

    foreach(QString feature, m_features)
    {
        qDebug() << "Getting file " << QString("%1%2").arg(m_updateSite).arg(feature);
        m_downloader.get(QString("%1%2").arg(m_updateSite).arg(feature));
    }

    siteXml->close();
    delete siteXml;
}

void ConsoleApplication::slotFeatureDownloadFinished(QBuffer *data)
{
    qDebug() << "Feature downloaded";

    m_amount_features.fetch_add(1);
    qDebug() << m_amount_features;

    QByteArray *feature = getFileFromZip("feature.xml", data);

    if(data->isOpen())
        data->close();
    delete data;

    delete feature;
}

QByteArray * ConsoleApplication::getFileFromZip(QString file, QBuffer *zip)
{
    QuaZip zipFile(zip);
    zipFile.open(QuaZip::mdUnzip);
    QuaZipFileInfo info;
    QuaZipFile aFile(&zipFile);

    bool more = zipFile.goToFirstFile();

    if(!more)
    {
        qDebug() << "The jar file doesn't contain any files.";
        exit(1);
    }

    while(more)
    {
        zipFile.getCurrentFileInfo(&info);
        aFile.open(QIODevice::ReadOnly);
        QString name = aFile.getActualFileName();

        if(name == file)
            break;

        more = zipFile.goToNextFile();
        aFile.close();
    }

    QByteArray * xmlContent = new QByteArray();
    xmlContent->append(aFile.readAll());
    aFile.close();
    return xmlContent;
}


void ConsoleApplication::slotPluginDownloadFinished(QBuffer *data)
{
    data->open(QIODevice::ReadOnly);
    QByteArray ba = data->readAll();
    data->close();
    delete data;

    qDebug() << QString("Download finished:\n=================\n%1").arg(ba.constData());
}
