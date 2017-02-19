#include "Lib.h"

String Lib::ReadTXT(String path,int line)
{
    QFile PreQFile(path);
    PreQFile.open(QIODevice::ReadOnly);
    QTextStream text(&PreQFile);
    String concert;
    if(line==-1)
    {concert=text.readAll();}
    else
    {concert=text.readLine(line);}
    PreQFile.close();
    return concert;
}

void Lib::WriteTXT(String path, String text)
{
   QFile PreQFile(path);
   PreQFile.open(QFile::Text|QFile::Append);
   QTextStream out(&PreQFile);
   out<<text;
   PreQFile.close();
}

void Lib::WriteINI(String path, String section, String var, String value)
{
   QSettings PreQSet(path,QSettings::IniFormat);
   PreQSet.setValue("/"+section+"/"+var,value);
}

String Lib::ReadINI(String path, String section, String var)
{
    QSettings PreQSet(path,QSettings::IniFormat);
    return PreQSet.value("/"+section+"/"+var).toString();
}

String Lib::getPath(String str)
{
   String path;
   QDir dir;
   path=dir.currentPath();
   path+="/"+str;
   return path;
}

void Lib::output(String content, bool lineFeed)
{
    if(lineFeed)
        cout<<content<<endl;
    else
        cout<<content;
}

String Lib::input(String information)
{
    output(information);
    String content;
    cin>>content;
    return content;
}
