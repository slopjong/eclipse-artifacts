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

#include "application.h"
#include "mainwindow.h"
#include "quazip.h"
#include "quazipfile.h"

Application::Application(int argc, char *argv[]) :
    QApplication(argc, argv),
    m_amount_features(0),
    m_amount_plugins(0),
    m_amount_processed_features(0),
    m_amount_processed_plugins(0),
    m_queryLanguage(QXmlQuery::XQuery10),
    m_gui_mode(false)
{
    // connect some event handlers with the download managers, one for each type
    // such a type is informally 'site.xml', 'feature' and 'plugin'
    connect(&m_site_downloader, SIGNAL(downloadFinished(QBuffer*, QString)),
            SLOT(slotUpdatesiteDownloadFinished(QBuffer*, QString)));
    connect(&m_feature_downloader, SIGNAL(downloadFinished(QBuffer*, QString)),
            SLOT(slotFeatureDownloadFinished(QBuffer*, QString)));
    connect(&m_plugin_downloader, SIGNAL(downloadFinished(QBuffer*, QString)),
            SLOT(slotPluginDownloadFinished(QBuffer*, QString)));
    connect(&m_head_request, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotHeadRequestFinished(QNetworkReply*)));

    // init the PKGBUILD variable templates which are written to the final PKGBUILD
    initVariables();

    if(argc>1)
    { // console mode

        QString updateSite = argv[1];
        updateSite = sanitizeUpdatesite(updateSite);
        m_pkgbuild_variables.insert("UPDATESITE", updateSite);        
    }
    else
    { // gui mode
        qDebug() << "No update site provided. Starting in gui mode.";
        m_gui_mode = true;
    }

}

Application::~Application()
{
    delete m_gui;
}

void Application::process()
{
    QTextStream cin(stdin, QIODevice::ReadOnly);
    QTextStream cout(stdout, QIODevice::WriteOnly);    // add the update site to the PKGBUILD variables

    if(m_gui_mode)
    {
        m_gui = new MainWindow;
        m_gui->initInput(m_pkgbuild_variables);
        m_gui->show();

        // connect some event handlers to the gui
        connect(m_gui, SIGNAL(updatesiteChanged(QString)), this, SLOT(slotUpdatesiteChanged(QString)));
        connect(this, SIGNAL(updatesiteValid()), m_gui, SLOT(slotUpdatesiteValid()));
        connect(this, SIGNAL(updatesiteInvalid()), m_gui, SLOT(slotUpdatesiteInvalid()));
        connect(this, SIGNAL(updatesiteLoading()), m_gui, SLOT(slotUpdatesiteLoading()));

        connect(this, SIGNAL(progressChanged(int)), m_gui, SLOT(slotProgressChanged(int)));
        connect(this, SIGNAL(progressMaximumChanged(int)), m_gui, SLOT(slotProgressMaxChanged(int)));

        connect(this, SIGNAL(downloadsFinished()), m_gui, SLOT(slotShowGenerateButton()));
        connect(m_gui, SIGNAL(generatePkgbuild()), this, SLOT(slotCreatePkgbuild()));
        connect(m_gui, SIGNAL(inputChanged(QHash<QString,QString>)),
                this, SLOT(slotSetPkgbuildVariables(QHash<QString,QString>)));
    }
    else
    {
        connect(this, SIGNAL(downloadsFinished()),
                this, SLOT(slotCreatePkgbuild()));

        m_pkgbuild_variables.insert("UPDATESITE", "");

        cout
        << endl
        << "Please provide some information about the eclipse plugin you're packaging."
        << endl
        << "The fields providing a default value can be left blank."
        << endl
        << endl;

        foreach(QString variable, variableTemplates())
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

        QString url = m_pkgbuild_variables.value("UPDATESITE");
        url.append("site.xml");
        m_site_downloader.get(url);
    }
}

/***************************
 * SLOTS
 ***************************/



void Application::slotUpdatesiteDownloadFinished(QBuffer *siteXml, QString fileName)
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
    emit progressMaximumChanged(m_amount_features + m_amount_plugins);

    m_features << features;

    QString updateSite = m_pkgbuild_variables.value("UPDATESITE");
    foreach(QString feature, features)
        m_feature_downloader.get(QString("%1%2").arg(updateSite).arg(feature));

    siteXml->close();
    delete siteXml;
}

void Application::slotFeatureDownloadFinished(QBuffer *data, QString fileName)
{
    m_amount_processed_features.fetch_add(1);
    emit progressChanged(m_amount_processed_features + m_amount_processed_plugins);

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
    emit progressMaximumChanged(m_amount_features + m_amount_plugins);

    m_plugins << plugins;

    QString updateSite = m_pkgbuild_variables.value("UPDATESITE");

    foreach(QString plugin, plugins)
    {
        QString downloadUrl = updateSite;
        downloadUrl.append(plugin);

        m_plugin_downloader.get(downloadUrl);
    }

    featureDocument.close();
}


void Application::slotPluginDownloadFinished(QBuffer *data, QString fileName)
{
    data->open(QIODevice::ReadOnly);
    QByteArray bytes = data->readAll();
    calculateHashes(fileName, bytes);

    data->close();
    delete data;

    m_amount_processed_plugins.fetch_add(1);
    emit progressChanged(m_amount_processed_features + m_amount_processed_plugins);

    qDebug() << QString("Plugin downloaded: %1/%2 => %3")
                .arg(m_amount_processed_plugins)
                .arg(m_amount_plugins)
                .arg(fileName);

    if(isDownloadsFinished())
        emit downloadsFinished();
}

void Application::slotCreatePkgbuild()
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
            QStringList deps = value.split(QRegExp("\\s+"));
            value = "";
            foreach(QString dep, deps)
            {
                // strip whitespaces
                dep = dep.replace(QRegExp("\\s+"), "");
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

    // if we are in console mode quit now
    if(!m_gui_mode)
        quit();
}

void Application::slotUpdatesiteChanged(QString updateSite)
{
    emit updatesiteLoading();
    updateSite = sanitizeUpdatesite(updateSite);
    updateSite.append("site.xml");
    QNetworkRequest req(updateSite);
    m_head_request.head(req);
}

void Application::slotHeadRequestFinished(QNetworkReply *reply)
{
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int iStatusCode = statusCode.toInt();

    if(iStatusCode == 200)
    {
        emit updatesiteValid();
        QString url = reply->url().toString();
        m_site_downloader.get(url);
        QString updateSite = url.replace("site.xml", "");
        m_pkgbuild_variables.insert("UPDATESITE", updateSite);
    }
    else
        emit updatesiteInvalid();

    reply->deleteLater();
}

void Application::slotSetPkgbuildVariables(QHash<QString, QString> input)
{
    m_pkgbuild_variables.unite(input);
}

/***************************
 * HELPERS
 ***************************/

void Application::calculateHashes(QString file, QByteArray &data)
{
    QString md5 = QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
    QString sha = QString(QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex());
    md5_hashes.insert(file, md5);
    sha1_hashes.insert(file, sha);
}

QByteArray Application::getFileFromZip(QString file, QBuffer *zip)
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

bool Application::isDownloadsFinished()
{
   bool pluginsReady = m_amount_plugins == m_amount_processed_plugins;
   bool featuresReady = m_amount_features == m_amount_processed_features;
   return pluginsReady && featuresReady;
}

QStringList Application::variableTemplates()
{
    QStringList variables;
    variables << "MAINTAINER" << "EMAIL" << "PKGNAME" << "PKGVER" << "PKGREL"
              << "DESCRIPTION" << "URL" << "LICENSE" << "ECLIPSEVER" << "DEPENDS";

    return variables;
}

void Application::initVariables()
{
    // fill all variables with empty values
    foreach(QString variable, variableTemplates())
        m_pkgbuild_variables.insert(variable, "");

    // set default values for some specific variables
    m_pkgbuild_variables.insert("ECLIPSEVER", "3.5");
    m_pkgbuild_variables.insert("LICENSE", "custom");
    m_pkgbuild_variables.insert("PKGREL", "1");

    // add the update site to the PKGBUILD variables
    // because it's not in the variable template list
    // because the user won't be asked for it during
    // the cli input but is required to pass it as an
    // argument
    m_pkgbuild_variables.insert("UPDATESITE", "");
}

QString Application::sanitizeUpdatesite(QString updateSite)
{
    // append a trailing slash if it's missing
    int size = updateSite.size();
    if(updateSite[size-1] != '/')
        updateSite.append("/");

    // prepend a http:// if missing
    if(!updateSite.contains("http"))
        updateSite.prepend("http://");

    return updateSite;
}
