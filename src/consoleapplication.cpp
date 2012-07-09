#include <QDebug>

#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QCryptographicHash>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include <QtXmlPatterns/QXmlNodeModelIndex>
#include <QtXmlPatterns/QSimpleXmlNodeModel>
#include <QtXmlPatterns/QXmlResultItems>

#include <cstdlib>
#include <typeinfo>
#include <iostream>
#include <string>

#include "quazip.h"
#include "quazipfile.h"

#include "consoleapplication.h"

ConsoleApplication::ConsoleApplication(int argc, char *argv[]) :
    QCoreApplication(argc, argv),
    m_amount_features(0),
    m_amount_plugins(0),
    m_amount_processed_features(0),
    m_amount_processed_plugins(0),
    m_queryLanguage(QXmlQuery::XQuery10)
{
    connect(&m_site_downloader, SIGNAL(downloadFinished(QBuffer*, QString)), SLOT(slotUpdatesiteDownloadFinished(QBuffer*, QString)));
    connect(&m_feature_downloader, SIGNAL(downloadFinished(QBuffer*, QString)), SLOT(slotFeatureDownloadFinished(QBuffer*, QString)));
    connect(&m_plugin_downloader, SIGNAL(downloadFinished(QBuffer*, QString)), SLOT(slotPluginDownloadFinished(QBuffer*, QString)));
    connect(this, SIGNAL(createPKGBUILD()), this, SLOT(slotDownloadsFinished()));

    if(argc>1)
        m_updateSite = argv[1];
    else
    {
        qDebug() << "Please provide an update site";
        std::exit(1);
    }

    // append a trailing slash if it's missing
    int size = m_updateSite.size();
    if(m_updateSite[size-1] != '/')
        m_updateSite.append("/");

    // add the update site to the PKGBUILD variables
    m_pkgbuild_variables.insert("UPDATESITE", m_updateSite);

    // fill some default values
    m_pkgbuild_variables.insert("PKGREL", "1");
    m_pkgbuild_variables.insert("LICENSE", "custom");
    m_pkgbuild_variables.insert("ECLIPSEVER", "3.5");
}

void ConsoleApplication::process()
{
    QTextStream cin(stdin, QIODevice::ReadOnly);
    QTextStream cout(stdout, QIODevice::WriteOnly);

    cout << "Please provide some information about the eclipse plugin you're packaging." << endl;

    QStringList variables;
    variables << "MAINTAINER" << "EMAIL" << "PKGNAME" << "PKGVER" << "PKGREL"
              << "DESCRIPTION" << "URL" << "LICENSE" << "ECLIPSEVER" << "DEPENDS";

    foreach(QString variable, variables)
    {
        QString value = m_pkgbuild_variables.value(variable);

        QString defaultMsg = "";

        if(value != "")
            defaultMsg = QString(" (default=%1) ").arg(value);

        if(variable == "PKGNAME")
            defaultMsg = QString(" (add the eclipse- prefix) ");

        if(variable == "DEPENDS")
            defaultMsg = QString(" (list plugins only, space-seperated) ");

        cout << variable.toLower().toLocal8Bit().constData() << defaultMsg << ": ";
        cout.flush();

        value = cin.readLine();

        if(value != "")
            m_pkgbuild_variables.insert(variable, value);
    }

    QString url = m_updateSite;
    url.append("site.xml");
    m_site_downloader.get(url);
}

/***************************
 * SLOTS
 ***************************/

void ConsoleApplication::slotUpdatesiteDownloadFinished(QBuffer *siteXml, QString fileName)
{
    qDebug() << "Downloaded site.xml from the update site.";

    siteXml->open(QIODevice::ReadOnly);

    QXmlQuery query(m_queryLanguage);
    query.bindVariable("inputDocument", siteXml);
    query.setQuery("doc($inputDocument)//feature");

    if (!query.isValid())
    {
        qDebug() << "Query invalid.";
        return;
    }

    QXmlResultItems results;
    query.evaluateTo(&results);

    QStringList features;

    QXmlItem item(results.next());
    while(!item.isNull())
    {
        QXmlQuery tmpQuery(m_queryLanguage);

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
                features << el;
        }

        item = results.next();
    }

    m_amount_features.fetch_add(features.size());
    m_features << features;

    foreach(QString feature, features)
        m_feature_downloader.get(QString("%1%2").arg(m_updateSite).arg(feature));

    siteXml->close();
    delete siteXml;
}

void ConsoleApplication::slotFeatureDownloadFinished(QBuffer *data, QString fileName)
{
    qDebug() << QString("Feature downloaded: %1").arg(fileName);

    if(!data->open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open the feature file in read mode." << endl;
        std::exit(1);
    }

    QByteArray feature = data->readAll();

    calculateHashes(fileName, feature);
    feature = getFileFromZip("feature.xml", data);

    if(data->isOpen())
        data->close();
    delete data;

    QBuffer featureDocument(&feature);
    featureDocument.open(QIODevice::ReadOnly);

    QXmlQuery query(m_queryLanguage);
    query.bindVariable("inputDocument", &featureDocument);
    query.setQuery("doc($inputDocument)//plugin");

    if (!query.isValid())
    {
        qDebug() << "Query invalid.";
        return;
    }

    QXmlResultItems results;
    query.evaluateTo(&results);

    QStringList plugins;

    QXmlItem item(results.next());
    while(!item.isNull())
    {
        QXmlQuery tmpQuery(m_queryLanguage);

        tmpQuery.bindVariable("pluginNode", item);
        QStringList tmpResults;

        QStringList attributeList;
        attributeList << "id" << "version";
        foreach(QString attribute, attributeList)
        {

            QString queryString = "$pluginNode/@" + attribute + "/string()";
            tmpQuery.setQuery(queryString);

            if (!tmpQuery.isValid())
            {
                qDebug() << "Sub-query invalid.";
                return;
            }


            if(!tmpQuery.evaluateTo(&tmpResults))
            {
                qDebug() << "Could not evaluate the sub-query.";
                return;
            }
        }

        QString pluginResource;
        pluginResource.prepend("plugins/");
        pluginResource.append(tmpResults.join("_"));
        pluginResource.append(".jar");

        plugins << pluginResource;

        item = results.next();
    }

    m_amount_plugins.fetch_add(plugins.size());
    m_plugins << plugins;

    foreach(QString plugin, plugins)
    {
        QString downloadUrl = m_updateSite;
        downloadUrl.append(plugin);

        m_plugin_downloader.get(downloadUrl);
    }

    featureDocument.close();

    m_amount_processed_features.fetch_add(1);
}


void ConsoleApplication::slotPluginDownloadFinished(QBuffer *data, QString fileName)
{
    data->open(QIODevice::ReadOnly);
    QByteArray bytes = data->readAll();
    calculateHashes(fileName, bytes);

    data->close();
    delete data;

    m_amount_processed_plugins.fetch_add(1);

    qDebug() << QString("Plugin downloaded: %1/%2 => %3")
                .arg(m_amount_processed_plugins)
                .arg(m_amount_plugins)
                .arg(fileName);

    if(downloadsFinished())
        emit createPKGBUILD();
}

void ConsoleApplication::slotDownloadsFinished()
{
    QFile file(":/PKGBUILD");
    file.open(QIODevice::ReadOnly);
    QByteArray pkgbuild = file.readAll();
    file.close();

    foreach(QString key, m_pkgbuild_variables.keys())
    {
        QString value = m_pkgbuild_variables.value(key);

        // the dependencies need some extra attention
        if(key == "DEPENDS")
        {
            QStringList deps = value.split(QRegExp("\s*"));
            value = "";
            foreach(QString dep, deps)
            {
                // strip whitespaces
                dep = dep.replace(QRegExp("\s*"), "");
                if(dep != "" && dep != " ")
                    value += QString("'%1' ").arg(dep);
            }
        }

        pkgbuild.replace("$"+key, value.toUtf8());
    }

    // now assemble the source, noextract and md5sums arrays

    QString sources = "source=(\n";
    QString noextract = "noextract=(\n";
    QString md5sums = "md5sums=(\n";

    QStringList artifacts;
    m_features.sort();
    m_plugins.sort();
    artifacts << m_features << m_plugins;

    foreach(QString artifact, artifacts)
    {
        sources += QString("\t${_updatesite}'%1' \n").arg(artifact);

        // strip the plugins/ and features/ part from the artifact string
        artifact.replace("plugins/", "");
        artifact.replace("features/", "");

        noextract += QString("\t'%1' \n").arg(artifact);
        QString md5 = md5_hashes.value(artifact);
        md5sums += QString("\t'%1' \n").arg(md5);
    }

    sources.append(")\n");
    noextract.append(")\n");
    md5sums.append(")\n");

    pkgbuild.replace("$SOURCES", sources.toUtf8());
    pkgbuild.replace("$NOEXTRACT", noextract.toUtf8());
    pkgbuild.replace("$MD5SUMS", md5sums.toUtf8());

    QFile output;
    output.setFileName("PKGBUILD");
    if(!output.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Could not open the file in write mode." << endl;
        std::exit(1);
    }
    QTextStream fout(&output);
    fout << pkgbuild << endl;
    output.close();

    qDebug() << "PKGBUILD created";
    quit();
}

/***************************
 * HELPERS
 ***************************/

void ConsoleApplication::calculateHashes(QString file, QByteArray &data)
{
    QString md5 = QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
    QString sha = QString(QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex());
    md5_hashes.insert(file, md5);
    sha1_hashes.insert(file, sha);
}

QByteArray ConsoleApplication::getFileFromZip(QString file, QBuffer *zip)
{
    QuaZip zipFile(zip);
    zipFile.open(QuaZip::mdUnzip);
    QuaZipFileInfo info;
    QuaZipFile aFile(&zipFile);

    bool more = zipFile.goToFirstFile();

    if(!more)
    {
        qDebug() << "The jar file doesn't contain any files.";
        std::exit(1);
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

    QByteArray xmlContent;// = new QByteArray();
    xmlContent.append(aFile.readAll());
    aFile.close();
    return xmlContent;
}

bool ConsoleApplication::downloadsFinished()
{
   bool pluginsReady = m_amount_plugins == m_amount_processed_plugins;
   bool featuresReady = m_amount_features == m_amount_processed_features;
   return pluginsReady && featuresReady;
}
