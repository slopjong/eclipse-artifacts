#include <QCoreApplication>
#include <QtCore/QStringList>
#include <Qxml>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    if(argc>1)
    {
        QString file = a.arguments().at(1);
    }

    return a.exec();
}
