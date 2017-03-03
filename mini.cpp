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

String MINI::getBlockName(String &str)
{
    String ret=preprocessor::getToken(str,vector<String>(" ",">"));
    str=str.right(str.length()-ret.length()); //消耗字符
    return ret;
}

//直读函数
String MINI::readVar(vector<String> layer, String var)
{
    typedef pair<String,bool> SI;
    vector<SI>stack; //栈中包含了是否需要跳过该区块的信息
    int nowLayerSub=0; //目前匹配到的层级（对应layer和stack的下标）

    for(String str:codelist)
    {
        preprocessor::removeChar(str," ");

        //对区块的处理
        if(preprocessor::removeChar(str,"<"))
        {
            if(preprocessor::removeChar(str,"/")) //检查是区块起始还是区块结尾
            {
                //是区块结尾
                String blockName=getBlockName(str);
                preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
                if(blockName!=stack.back().first) //检查和区块头是否对应
                {
                    preprocessor::mistake("区块结尾和最近的区块头不对应");
                    return NULL_String;
                }
                if(!preprocessor::isChar(str,">"))
                {
                    preprocessor::mistake("区块尾不能含有除区块名外其他元素");
                    return NULL_String;
                }
                stack.pop_back(); //似乎直接弹栈即可
            }
            else
            {
                //是区块起始
                String blockName=getBlockName(str);
                preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格

                if(stack.size()<layer.size()&&blockName==layer[nowLayerSub+1]) //检查这个区块是否是目前要匹配的下级区块（如果目前已经匹配到了也一样跳过）
                {
                    nowLayerSub++;
                    stack.push_back(SI(blockName,false,));
                }
                else
                {stack.push_back(SI(blockName,true));} //如果不是需要的区块，准备跳过它

                if(!preprocessor::isChar(str,">")) //检查该区块是否有参数
                {
                    //fix me:提取参数的代码，生成分析树的时候再补充吧！
                }
            }
            continue;
        }

        //对field的处理
        if(str.indexOf("=")!=-1)
        {
            if(stack.size()!=0&&stack.back().second) //检查该区块是否需要跳过
            {continue;} //需要跳过就跳过

            if(stack.size()!=layer.size()) //检查是否已经到达要查找的区块的层级
            {continue;} //不是就跳过

            QStringList varlist=str.split("=");
            if(varlist[0]==var)
            {return varlist[1];}

            continue;
        }

        //什么都不是
        continue;
    }
}
