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

    QString file = "resources/artifacts.xml";

    sourceDocument.setFileName(file);

    if(sourceDocument.open(QIODevice::ReadOnly))
    {
        QXmlQuery query;
        query.bindVariable("inputDocument", &sourceDocument);
        query.setQuery("doc($inputDocument)/repository/mappings/rule");

        if (!query.isValid())
        {
            cout << "query valid" << endl;
            exit(1)
        }

        QXmlResultItems results;
        query.evaluateTo(&results);
        const QXmlItem item = results.next();

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

        tmpQuery.bindVariable("ruleNode", item);
        tmpQuery.setQuery("$ruleNode/filter");

        if (!tmpQuery.isValid())
        {
            cout << "tmpquery not valid" << endl;
            exit(1);
        }

        QXmlResultItems tmpResults;
        tmpQuery.evaluateTo(&tmpResults);
        const QXmlItem tmpItem = tmpResults.next();

        if (tmpItem.isNull())
        {
            cout << "tmpquery is null" << endl;
            exit(1);
        }
        else if( tmpItem.isNode())
        {
            cout << "tmpitem is node" << endl;
        }
        else if( tmpItem.isAtomicValue())
        {
            cout << "tmpitem is atomic" << endl;
        }

        QVariant atomic = tmpItem.toAtomicValue();
        cout << atomic.typeName() << endl; // outputs QString
        cout << tmpItem.toAtomicValue().toString() << endl; // is empty
    }
    else
        cout << "Could not read the XML document" << endl;

    exit(0);
    return a.exec();
}
