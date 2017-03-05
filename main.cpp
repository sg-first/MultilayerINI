#include <QCoreApplication>
#include "mini.h"

Lib *aLib;

int main(int argc, char *argv[])
{
    /*QCoreApplication a(argc, argv);
    return a.exec();*/
    aLib=new Lib;
    aLib->WriteTXT("D:/isgo.txt","aaa");
}
