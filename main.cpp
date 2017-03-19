#include <QCoreApplication>
#include "mini.h"

Lib *aLib;

int main(int argc, char *argv[])
{
    /*QCoreApplication a(argc, argv);
    return a.exec();*/
    aLib=new Lib;
    MINI mymini("D:/123.mini");
    mymini.toTree();
    return 0;
}
