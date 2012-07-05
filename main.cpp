#include <QCoreApplication>

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


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

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

    QIODevice * sourceDocument = &aFile;

    QXmlQuery query(QXmlQuery::XPath20);
    query.bindVariable("inputDocument", sourceDocument);
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

    sourceDocument->close();

    exit(0);
    return a.exec();
}
