
#include "consoleapplication.h"

int main(int argc, char *argv[])
{
    ConsoleApplication a(argc, argv);
    a.process();
    return a.exec();
}
