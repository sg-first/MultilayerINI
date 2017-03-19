#include "mini.h"
#include "preprocessor.h"

MINI::MINI(String content, bool ispath)
{
    if(ispath)
    {String content=aLib->ReadTXT(content);}
    this->setCode(content);
}

void MINI::initParsetree()
{
    delete parsetree;
    parsetree=nullptr;
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

String MINI::getParName(String &str)
{
    String ret=preprocessor::getToken(str,vector<String>(" ","="));
    str=str.right(str.length()-ret.length()); //消耗字符
    return ret;
}

String MINI::getParVal(String &str)
{return getBlockName(str);}

bool MINI::blockEnd(String &str,vector<SI> &stack)
{
    String blockName=getBlockName(str);
    preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
    if(blockName!=stack.back().first) //检查和区块头是否对应
    {
        preprocessor::mistake("区块结尾和最近的区块头不对应");
        return false;
    }
    if(!preprocessor::isChar(str,">"))
    {
        preprocessor::mistake("区块尾不能含有除区块名外其他元素");
        return false;
    }
    if(!stack.back().second) //检查是否要弹出待查区块（不准备跳过的区块）
    {return false;} //如果待查区块被弹出，说明无法向下一级搜索，即没有这个var
    stack.pop_back(); //似乎直接弹栈即可
    return true;
}

void MINI::blockBegin(String &str, vector<SI> &stack, vector<String> &layer,int &nowLayerSub)
{
    String blockName=getBlockName(str);
    preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格

    if(stack.size()<layer.size()&&blockName==layer[nowLayerSub+1]) //检查这个区块是否是目前要匹配的下级区块（如果目前已经匹配到了也一样跳过）
    {
        nowLayerSub++;
        stack.push_back(SI(blockName,false));
    }
    else
    {stack.push_back(SI(blockName,true));} //如果不是需要的区块，准备跳过它
}

//直读函数
String MINI::readVar(vector<String> layer, String var)
{
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
                if(!blockEnd(str,stack))
                {return NULL_String;}
            }
            else
            {
                //是区块起始
                blockBegin(str,stack,layer,nowLayerSub);
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
    }
    return NULL_String;
}

String MINI::readPar(vector<String> layer, String par)
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
                if(!blockEnd(str,stack))
                {return NULL_String;}
            }
            else
            {
                //是区块起始
                blockBegin(str,stack,layer,nowLayerSub);

                if(!preprocessor::isChar(str,">")) //检查该区块是否有参数
                {
                    if(stack.back().second) //检查该区块是否需要跳过（由于前面刚刚压栈，所以这里长度不需要检查）
                    {continue;} //需要跳过就跳过

                    if(stack.size()!=layer.size()) //检查是否已经到达要查找的区块的层级
                    {continue;} //不是就跳过

                    while(1) //循环获取区块参数
                    {
                        String parName=getParName(str);
                        preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
                        if(!preprocessor::removeChar(str,"="))
                        {
                            preprocessor::mistake("每个区块参数必须有一个值");
                            return NULL_String;
                        }
                        preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
                        String parVal=getParVal(str);
                        preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格

                        if(parName==par) //检查是否是想要的
                        {return parVal;}

                        if(preprocessor::isChar(str,">"))
                            break;
                        if(str.length()==0)
                        {
                            preprocessor::mistake("区块头必须以>作为结尾");
                            return NULL_String;
                        }
                    }
                    return NULL_String; //给定区块查不到
                }
            }
            continue;
        }

        //别的case都不管
    }
    return NULL_String;
}

String MINI::getCode(vector<String> layer)
{
    vector<SI>stack; //栈中包含了是否需要跳过该区块的信息
    int nowLayerSub=0; //目前匹配到的层级（对应layer和stack的下标）
    String newcode="";
    bool inBlock=false;

    for(String str:codelist)
    {
        String oriStr=str; //没有消耗过字符的该句
        preprocessor::removeChar(str," ");

        //对区块的处理
        if(!inBlock&&preprocessor::removeChar(str,"<"))
        {
            if(preprocessor::removeChar(str,"/")) //检查是区块起始还是区块结尾
            {
                //是区块结尾
                if(inBlock)
                {
                    String blockName=getBlockName(str);
                    if(blockName==layer.back()) //准备弹出要取内部代码的区块
                        return newcode;
                    newcode+=oriStr+"\n\r"; //区块结尾不算在内，所以后连接
                }

                if(!blockEnd(str,stack))
                {return NULL_String;}
            }
            else
            {
                //是区块起始
                blockBegin(str,stack,layer,nowLayerSub);
                if(inBlock)
                    newcode+=oriStr+"\n\r"; //区块开头不算在内，所以先连接
                if(stack.size()==layer.size())
                    inBlock=true;
            }
            continue;
        }

        //其它
        if(inBlock)
            newcode+=oriStr+"\n\r";
        continue;
    }
    return NULL_String;
}

String MINI::writeVar(vector<String> layer, String var, String val)
{
    vector<SI>stack; //栈中包含了是否需要跳过该区块的信息
    int nowLayerSub=0; //目前匹配到的层级（对应layer和stack的下标）
    String newcode="";
    bool finished=false;
    String addcode=var+"="+val;

    for(String str:codelist)
    {
        if(finished)
        {
            newcode+=str+"\n\r";
            continue;
        }
        else
        {
            String oriStr=str;
            preprocessor::removeChar(str," ");

            //对区块的处理
            if(preprocessor::removeChar(str,"<"))
            {
                if(preprocessor::removeChar(str,"/")) //检查是区块起始还是区块结尾
                {
                    //是区块结尾
                    String blockName=getBlockName(str);
                    if(blockName==layer.back()) //准备弹出要添加（修改）变量的区块
                    {
                        newcode+=addcode+"\n\r"+oriStr+"\n\r";
                        finished=true;
                        continue;
                    }

                    if(!blockEnd(str,stack))
                    {return NULL_String;}
                }
                else
                {
                    //是区块起始
                    blockBegin(str,stack,layer,nowLayerSub);
                }
                newcode+=oriStr+"\n\r";
                continue;
            }

            //对field的处理
            if(str.indexOf("=")!=-1)
            {
                if(stack.size()!=0&&stack.back().second) //检查该区块是否需要跳过
                {
                    newcode+=oriStr+"\n\r";
                    continue;
                }

                if(stack.size()!=layer.size()) //检查是否已经到达要查找的区块的层级
                {
                    newcode+=oriStr+"\n\r";
                    continue;
                }

                QStringList varlist=str.split("=");
                if(varlist[0]==var)
                {
                    newcode+=addcode+"\n\r";
                    finished=true;
                    continue;
                }
            }

            //什么都不是
            newcode+=oriStr+"\n\r";
            continue;
        }
    }

    setCode(newcode);
    initParsetree();
    return newcode;
}

void MINI::writeVarToFile(vector<String> layer, String var, String val, String path)
{
    aLib->RemoveFile(path);
    aLib->WriteTXT(path,writeVar(layer,var,val));
}

Tree *MINI::toTree()
{
    vector<String>stack;
    Tree* nowNode=new Tree(""); //根节点没名字
    this->parsetree=nowNode;

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
                if(blockName!=stack.back()) //检查和区块头是否对应
                {
                    preprocessor::mistake("区块结尾和最近的区块头不对应");
                    initParsetree();
                    return nullptr;
                }
                if(!preprocessor::isChar(str,">"))
                {
                    preprocessor::mistake("区块尾不能含有除区块名外其他元素");
                    initParsetree();
                    return nullptr;
                }
                //似乎直接弹栈即可
                stack.pop_back();
                nowNode=nowNode->getFather();
            }
            else
            {
                String blockName=getBlockName(str);
                preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
                stack.push_back(blockName);

                Tree* childNode=new Tree(blockName,nowNode);
                nowNode->addChildNode(childNode);
                nowNode=childNode;

                if(!preprocessor::isChar(str,">")) //检查该区块是否有参数
                {
                    while(1) //循环获取区块参数
                    {
                        String parName=getParName(str);
                        preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
                        if(!preprocessor::removeChar(str,"="))
                        {
                            preprocessor::mistake("每个区块参数必须有一个值");
                            initParsetree();
                            return nullptr;
                        }
                        preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
                        String parVal=getParVal(str);
                        preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
                        nowNode->addPar(parName,parVal);

                        if(preprocessor::isChar(str,">"))
                            break;
                        if(str.length()==0)
                        {
                            preprocessor::mistake("区块头必须以>作为结尾");
                            initParsetree();
                            return nullptr;
                        }
                    }
                }
            }
            continue;
        }

        //对field的处理
        if(str.indexOf("=")!=-1)
        {
            QStringList varlist=str.split("=");
            nowNode->addField(varlist[0],varlist[1]);
            continue;
        }

        //什么都不是就过
    }

    return this->parsetree;
}

Tree *MINI::getTree() //调用之前需保证树被创建
{return this->parsetree;}

String MINI::getCode()
{
    String code="";
    for(String i:codelist)
    {code+=i+"\n\r";}
    return code;
}
