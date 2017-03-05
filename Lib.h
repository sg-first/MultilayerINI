#pragma once
#include <QString>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <iostream>
using namespace std;

typedef QString String;
#define NULL_String ""

class Lib : public QObject
{
    Q_OBJECT
public:
    Lib(const Lib&):QObject(0){}
    Lib():cout(stdout,QIODevice::WriteOnly),cin(stdin,QIODevice::ReadOnly){}

    Q_INVOKABLE String ReadTXT(String path, int line=-1);
    Q_INVOKABLE void WriteTXT(String path,String text);
    Q_INVOKABLE void WriteINI(String path,String section,String var,String value);
    Q_INVOKABLE String ReadINI(String path,String section,String var);
    Q_INVOKABLE String getPath(String str);
    Q_INVOKABLE void output(String content,bool lineFeed=true);
    Q_INVOKABLE String input(String information);
    Q_INVOKABLE void RemoveFile(String path);

private:
    QTextStream cout;
    QTextStream cin;
};
extern Lib *aLib;
