#include "preprocessor.h"

void preprocessor::deleteSpace(String &str) //把几个连一起的空格变成一个空格
{
    bool isSpace=false;
    bool isQuotes=false;
    String newstr="";

    for(auto i:str)
    {
        if(isQuotes) //前一个字符是引号
        {
            if(i=='"') //这个字符是反引号，出状态
                isQuotes=false;
            newstr+=i; //无论是否是反引号，这个字符都不会被去除
            continue;
        }
        //前一个不是引号就先检查这个是不是引号
        if(i=='"')
        {
            isQuotes=true; //进状态
            newstr+=i;
            continue;
        }
        //确定跟引号没关系了再检查空格
        if(i==' ')
        {
            if(isSpace) //前一个已经是空格了，这个直接忽略
                continue;
            //前一个不是空格，进状态并写入空格
            isSpace=true;
            newstr+=i;
            continue;
        }
        //普通字符
        isSpace=false; //无论如何都出空格状态
        newstr+=i;
    }
    str=newstr;
}

bool preprocessor::removeChar(String &str, String Char)
{
    if(isChar(str,Char))
    {
        str.right(str.length()-1);
        return true;
    }
    return false;
}

bool preprocessor::isChar(String str, String Char)
{return str.left(1)==Char;}

String preprocessor::getToken(String str, vector<String> terminatorList)
{
    String newstr="";

    for(auto i:str)
    {
        for(String j:terminatorList)
        {
            if(i==j)
                return newstr;
        }
        //不是终结符，直接拼接即可
        newstr+=i;
    }
    mistake("规约区块名时没有遇到终结符");
    return NULL_String;
}

void preprocessor::mistake(String information)
{aLib->output("error:"+information);}

int preprocessor::getLen(String literal)
{return literal.length();}
