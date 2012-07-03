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

void m1();
void m2();
void m3();

QTextStream cout(stdout, QIODevice::WriteOnly);
QFile sourceDocument;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	if(argc>0)
    {
		QString file = "";// a.arguments().at(1);
		file = "resources/artifacts.xml";

		sourceDocument.setFileName(file);

		if(sourceDocument.open(QIODevice::ReadOnly))
		{
            m3();
		}
		else
			cout << "Could not read the XML document" << endl;
    }
	else
		cout << "No file passed." << endl;

	exit(0);
	return a.exec();
}

void m1()
{
	QXmlQuery query;
	query.setFocus(&sourceDocument);
	query.setQuery("repository/mappings/rule");

	if (!query.isValid())
	{
		cout << "Not valid" << endl;
		exit(1);
	}

	QString rules;

	if(query.evaluateTo(&rules))
	{
		cout << rules << endl;
	}
	else
		cout << "Could not evaluate" << endl;
}

void m2()
{
	QXmlQuery query;
	query.bindVariable("inputDocument", &sourceDocument);
    //query.setQuery("doc($inputDocument)/repository/mappings/rule/@filter/string()");
    query.setQuery("doc($inputDocument)/repository/mappings/rule");

	if (!query.isValid())
	{
		cout << "Not valid" << endl;
		exit(1);
	}

	QXmlResultItems results;
    query.evaluateTo(&results);
    const QXmlItem item = results.next();

	if(!item.isNull())
	{
        cout << "is not null" << endl;

		if(item.isNode())
		{
            cout << "is node" << endl;

			QXmlQuery tmpQuery;
			QXmlResultItems tmpResult;
			QXmlItem tmp;
			tmpQuery.bindVariable("ruleNode", item);
            tmpQuery.setQuery("$ruleNode");

            if (!tmpQuery.isValid())
            {
                cout << "tmpquery not valid" << endl;
                exit(1);
            }

			tmpQuery.evaluateTo(&tmpResult);
            tmp = tmpResult.current();

			if(!tmp.isNull())
			{
				cout << "tmp not null" << endl;

				if (tmp.isAtomicValue())
					cout << "Atomic" << endl;
				else
					cout << "Not atomic" << endl;
			}
			else
				cout << "tmp null" << endl;
		}
        else if(item.isAtomicValue())
        {
            cout << "atomic" << endl;
            cout << item.toAtomicValue().toString() << endl;
        }
        else
            cout << "not a node and not atomic" << endl;
	}
	else
		cout << "Could not evaluate" << endl;
}

void m3()
{
    QXmlQuery query;
    query.bindVariable("inputDocument", &sourceDocument);
    query.setQuery("doc($inputDocument)/repository/mappings/rule");

    if (!query.isValid())
    {
        cout << "query valid" << endl;
        exit(1);
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
    tmpQuery.setQuery("$ruleNode/string()");

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
