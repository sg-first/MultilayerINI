#include <QCoreApplication>
#include "mini.h"

Lib *aLib;

int main()
{
    aLib=new Lib;
    MINI mymini("D:/123.mini");
    mymini.toTree();
    Tree* tree=mymini.getTree();
    MINI oth(tree);
    oth.toCode("D:/mycode.txt");
    return 0;
}
