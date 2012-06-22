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
			m2();
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
	//query.setFocus(&sourceDocument);
	query.bindVariable("inputDocument", &sourceDocument);
	query.setQuery("doc($inputDocument)//repository/mappings/rule");

	if (!query.isValid())
	{
		cout << "Not valid" << endl;
		exit(1);
	}

	QXmlResultItems results;
	query.evaluateTo(&results);
	QXmlItem item = results.next();
	if(!item.isNull())
	{
		cout << "isNull" << endl;

		if(item.isNode())
		{
			cout << "isNode" << endl;

			QXmlQuery tmpQuery;
			QXmlResultItems tmpResult;
			QXmlItem tmp;
			tmpQuery.bindVariable("ruleNode", item);
			tmpQuery.setQuery("$ruleNode/@filter/string()");
			tmpQuery.evaluateTo(&tmpResult);
			tmp = tmpResult.next();

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
		//cout << rules << endl;
	}
	else
		cout << "Could not evaluate" << endl;
}
