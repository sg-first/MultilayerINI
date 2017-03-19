#include <QCoreApplication>
#include "mini.h"

Lib *aLib;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    aLib=new Lib;
    MINI mymini("D:/123.mini");
    mymini.toTree();
    Tree* tree=mymini.getTree();
    MINI other(tree);
    other.toCode("D:/mycode.txt");

    String val=other.readParInTree(vector<String>{"name1","name2"},"sdsd");
    val=other.readVarInTree(vector<String>{"name1","name2"},"var4545");
    val=other.readPar(vector<String>{"name1","name2"},"sdsd");
    val=other.readVar(vector<String>{"name1","name2"},"var4545");
    return a.exec();
}
