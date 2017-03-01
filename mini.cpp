#include "mini.h"
#include "preprocessor.h"

MINI::MINI(String path)
{
    String code=aLib->ReadTXT(path);
    this->setCode(code);
}

void MINI::setCode(String code)
{
    this->codelist.clear();
    preprocessor::deleteSpace(code);
    QStringList ary=code.split("\r\n");
    for(String i:ary)
    {this->codelist.push_back(i);}
}

bool MINI::isParse()
{return !(this->parsetree==nullptr);}

String MINI::getBlockName(String str)
{return preprocessor::getToken(str,vector<String>(" ",">"));}

//直读函数
String MINI::readVar(vector<String> layer, String var)
{
    vector<String>stack;
    int nowLayerSub=0; //目前匹配到的层级（对应layer和stack的下标）

    for(String str:codelist)
    {
        if(str=="")
            continue;

        preprocessor::removeChar(str," ");
        if(preprocessor::isChar(str,"<"))
        {
            //是区块起始，判断区块名
        }
    }
}
