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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextStream cout(stdout, QIODevice::WriteOnly);
    QFile sourceDocument;

    QString file = "resources/feature.xml";

    sourceDocument.setFileName(file);

    if(sourceDocument.open(QIODevice::ReadOnly))
    {
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
    }
    else
        cout << "Could not read the XML document" << endl;

    exit(0);
    return a.exec();
}
