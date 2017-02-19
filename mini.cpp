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

String MINI::readVar(vector<String> layout, String var)
{
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
