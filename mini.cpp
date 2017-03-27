#include "mini.h"
#include "preprocessor.h"

MINI::MINI(String content, bool ispath)
{
    if(ispath)
    {content=aLib->ReadTXT(content);}
    this->setCode(content);
}

MINI::MINI(Tree *tree)
{this->parsetree=tree;}

MINI::~MINI()
{delete this->parsetree;}

MINIsta MINI::getState()
{
    if(this->parsetree!=nullptr&&this->codelist.size()!=0)
        return ALL;
    if(this->parsetree!=nullptr)
        return TREE;
    else
        return CODE;
}

void MINI::format()
{
    initParsetree();
    toTree();
    toCode();
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

String getBlockName(String &str)
{
    String ret=preprocessor::getToken(str,vector<String>{" ",">"});
    str=str.right(str.length()-ret.length()); //消耗字符
    return ret;
}

String getParName(String &str)
{
    String ret=preprocessor::getToken(str,vector<String>{" ","="});
    str=str.right(str.length()-ret.length()); //消耗字符
    return ret;
}

String getParVal(String &str)
{return getBlockName(str);}

bool blockEndMistakeCheck(String blockName,vector<SI>&stack,String &str) //已经消耗区块名后使用
{
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
    return true;
}

bool blockEndMistakeCheck(String blockName,vector<String>&stack,String &str) //已经消耗区块名后使用
{
    if(blockName!=stack.back()) //检查和区块头是否对应
    {
        preprocessor::mistake("区块结尾和最近的区块头不对应");
        return false;
    }
    if(!preprocessor::isChar(str,">"))
    {
        preprocessor::mistake("区块尾不能含有除区块名外其他元素");
        return false;
    }
    return true;
}

bool blockEnd(String &str,vector<SI> &stack)
{
    String blockName=getBlockName(str);
    preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格

    if(!blockEndMistakeCheck(blockName,stack,str))
        return false;

    if(!stack.back().second) //检查是否要弹出待查区块（不准备跳过的区块）
        return false; //如果待查区块被弹出，说明无法向下一级搜索，即没有这个var

    stack.pop_back(); //似乎直接弹栈即可
    return true;
}

String blockBegin(String &str, vector<SI> &stack, vector<String> &layer,unsigned int &nowLayerSub)
{
    String blockName=getBlockName(str);
    preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格

    if(stack.size()<layer.size()&&blockName==layer[nowLayerSub]) //检查这个区块是否是目前要匹配的区块（如果目前已经匹配到了目标区块也一样跳过）
    {
        nowLayerSub++;
        stack.push_back(SI(blockName,false));
    }
    else
    {stack.push_back(SI(blockName,true));} //如果不是需要的区块，准备跳过它

    return blockName;
}

inline bool isJump(vector<SI>&stack) //检查当前区块（back）是否需要跳过
{return stack.size()!=0&&stack.back().second;}

inline bool isBlock(String &str) //检查该str是否是区块
{return preprocessor::removeChar(str,"<");}

inline bool isBlockEnd(String &str) //检查该str（确认是区块后）是否是区块结尾
{return preprocessor::removeChar(str,"/");}

inline bool isField(String &str) //检查该str是否是field
{return str.indexOf("=")!=-1;}

inline bool isBlockHavePar(String &str) //检查该str（确认是区块起始后）是否含有参数，无副作用
{return !preprocessor::isChar(str,">");}

variable getaParInStr(String &str)
{
    String name=getParName(str);
    preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
    if(!preprocessor::removeChar(str,"="))
    {
        preprocessor::mistake("每个区块参数必须有一个值");
    }
    preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
    String value=getParVal(str);
    preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格

    return variable(name,value);
}

variable getVarInStr(String &str)
{
    QStringList varlist=str.split("=");
    return variable(varlist[0],varlist[1]);
}

//直读函数
String MINI::readVar(vector<String> layer, String var)
{
    vector<SI>stack; //栈中包含了是否需要跳过该区块的信息
    unsigned int nowLayerSub=0; //目前匹配到的层级（对应layer和stack的下标）

    for(String str:codelist)
    {
        preprocessor::removeChar(str," ");

        if(isBlock(str))
        {
            if(isBlockEnd(str))
            {
                if(!blockEnd(str,stack))
                {return NULL_String;}
            }
            else
            {blockBegin(str,stack,layer,nowLayerSub);}
            continue;
        }

        if(isField(str))
        {
            if(isJump(stack))
            {continue;}

            if(stack.size()!=layer.size()) //检查是否已经到达要查找的区块的层级
            {continue;} //不是就跳过

            variable fieldvar=getVarInStr(str);
            if(fieldvar.first==var)
            {return fieldvar.second;}

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
    unsigned int nowLayerSub=0; //目前匹配到的层级（对应layer和stack的下标）

    for(String str:codelist)
    {
        preprocessor::removeChar(str," ");

        if(isBlock(str))
        {
            if(isBlockEnd(str))
            {
                if(!blockEnd(str,stack))
                {return NULL_String;}
            }
            else
            {
                blockBegin(str,stack,layer,nowLayerSub);

                if(isBlockHavePar(str))
                {
                    if(isJump(stack))
                    {continue;}

                    if(stack.size()!=layer.size()) //检查是否已经到达要查找的区块的层级
                    {continue;} //不是就跳过

                    while(1) //循环获取区块参数
                    {
                        variable var=getaParInStr(str);

                        if(var.first==par) //检查是否是想要的
                        {return var.second;}

                        if(isBlockHavePar(str))
                            break;

                        //字符耗尽但无<
                        if(str.length()==0)
                        {
                            preprocessor::mistake("区块头必须以>作为结尾");
                            return NULL_String;
                        }
                    }
                    return NULL_String; //给定区块查不到所要的var
                }
            }
            continue;
        }

        //除区块外别的case都不管
    }
    return NULL_String;
}

String MINI::getBlockCode(vector<String> layer)
{
    vector<SI>stack; //栈中包含了是否需要跳过该区块的信息
    unsigned int nowLayerSub=0; //目前匹配到的层级（对应layer和stack的下标）
    String newcode="";
    bool inGetBlock=false; //是否在待拷贝代码的区块内

    for(String str:codelist)
    {
        String oriStr=str; //没有消耗过字符的该句
        preprocessor::removeChar(str," ");

        if(isBlock(str))
        {
            if(isBlockEnd(str))
            {
                if(inGetBlock)
                {
                    String blockName=getBlockName(str);

                    if(blockName==layer.back()) //将要弹出正在get的区块
                        return newcode; //get结束
                    else
                        newcode+=oriStr+"\r\n";
                }

                if(!blockEnd(str,stack))
                {return NULL_String;}
            }
            else
            {
                blockBegin(str,stack,layer,nowLayerSub);

                if(inGetBlock)
                {newcode+=oriStr+"\r\n";}
                else
                {
                    if(stack.size()==layer.size()) //已经到达要get的区块
                        inGetBlock=true;
                }
            }
            continue;
        }

        //其它
        if(inGetBlock)
            newcode+=oriStr+"\r\n";
        continue;
    }
    return NULL_String;
}

String getTAB(vector<String>&layer)
{
    String tab="";
    int num=layer.size();
    for(int i=1;i<=num;i++)
        tab+="\t";
    return tab;
}

String MINI::writeVar(vector<String> layer, String var, String val,String path)
{
    vector<SI>stack; //栈中包含了是否需要跳过该区块的信息
    unsigned int nowLayerSub=0; //目前匹配到的层级（对应layer和stack的下标）
    String newcode="";
    bool finished=false; //是否已经完成写变量
    String addcode=getTAB(layer)+var+"="+val;

    for(String str:codelist)
    {
        if(finished)
        {
            newcode+=str+"\r\n";
            continue;
        }
        else
        {
            String oriStr=str;
            preprocessor::removeChar(str," ");

            if(isBlock(str))
            {
                if(isBlockEnd(str))
                {
                    String blockName=getBlockName(str);

                    if(blockName==layer.back()) //准备弹出要添加（修改）变量的区块
                    {
                        newcode+=addcode+"\r\n"+oriStr+"\r\n";
                        finished=true;
                        continue;
                    }

                    if(!blockEnd(str,stack))
                    {return NULL_String;}
                }
                else
                {blockBegin(str,stack,layer,nowLayerSub);}

                newcode+=oriStr+"\r\n";
                continue;
            }

            if(isField(str))
            {
                if(isJump(stack)||(stack.size()!=layer.size())) //需要跳过或未到层级
                {
                    newcode+=oriStr+"\r\n";
                    continue;
                }

                QStringList varlist=str.split("=");
                if(varlist[0]==var)
                {
                    newcode+=addcode+"\r\n";
                    finished=true;
                    continue;
                }
            }

            //什么都不是
            newcode+=oriStr+"\r\n";
            continue;
        }
    }

    setCode(newcode);
    initParsetree();

    if(path!=NULL_String)
    {
        aLib->RemoveFile(path);
        aLib->WriteTXT(path,newcode);
    }

    return newcode;
}

String MINI::writePar(vector<String> layer, String var, String val, String path)
{
    vector<SI>stack; //栈中包含了是否需要跳过该区块的信息
    unsigned int nowLayerSub=0; //目前匹配到的层级（对应layer和stack的下标）
    String newcode="";
    bool finished=false;
    String addcode=var+"="+val;

    for(String str:codelist)
    {
        if(finished)
        {
            newcode+=str+"\r\n";
            continue;
        }
        else
        {
            String oriStr=str;
            preprocessor::removeChar(str," ");

            if(isBlock(str))
            {
                if(isBlockEnd(str))
                {
                    if(!blockEnd(str,stack))
                    {return NULL_String;}
                }
                else
                {
                    String blockName=blockBegin(str,stack,layer,nowLayerSub);

                    if(isJump(stack)||(stack.size()!=layer.size()))  //需要跳过或未到所查层级
                    {
                        newcode+=oriStr+"\r\n";
                        continue;
                    }

                    //要更改的参数就在这个区块中，先前调用blockBegin已经消耗了区块名和空格，下面直接循环检查参数即可
                    newcode+="<"+blockName+" "+addcode;
                    finished=true; //此时已完成
                    if(isBlockHavePar(str))
                    {
                        while(1) //循环连接区块的其它参数
                        {   
                            variable strvar=getaParInStr(str);
                            if(strvar.first!=var) //判断是否不是要找的那个par
                            {newcode+=" "+strvar.first+"="+strvar.second;} //不是就连接，是什么都不做（因为之前已经加到最前面了）

                            if(isBlockHavePar(str))
                                break;

                            //字符耗尽但无<
                            if(str.length()==0)
                            {
                                preprocessor::mistake("区块头必须以>作为结尾");
                                return NULL_String;
                            }
                        }
                    }
                    newcode+=">\r\n";
                    continue;
                }
            }

            //什么都不是
            newcode+=oriStr+"\r\n";
        }
    }

    setCode(newcode);
    initParsetree();

    if(path!=NULL_String)
    {
        aLib->RemoveFile(path);
        aLib->WriteTXT(path,newcode);
    }

    return newcode;
}

Tree *MINI::toTree() //调用之前需保证代码被创建
{
    initParsetree();
    vector<String>stack;
    Tree* nowNode=new Tree(NULL_String); //根节点没名字
    this->parsetree=nowNode;

    for(String str:codelist)
    {
        preprocessor::removeChar(str," ");

        if(isBlock(str))
        {
            if(isBlockEnd(str))
            {
                String blockName=getBlockName(str);
                preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格

                if(!blockEndMistakeCheck(blockName,stack,str))
                {return nullptr;}

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

                if(isBlockHavePar(str)) //检查该区块是否有参数
                {
                    while(1) //循环获取区块参数
                    {
                        nowNode->addPar(getaParInStr(str));

                        if(isBlockHavePar(str))
                            break;

                        //字符耗尽但无<
                        if(str.length()==0)
                        {
                            preprocessor::mistake("区块头必须以>作为结尾");
                            return nullptr;
                        }
                    }
                }
            }
            continue;
        }

        if(isField(str))
        {
            nowNode->addField(getVarInStr(str));
            continue;
        }

        //什么都不是就过
    }

    return this->parsetree;
}

Tree *MINI::getTree() //调用之前需保证树被创建
{return this->parsetree;}

String MINI::getCode(String path) //调用之前需保证代码被创建
{
    String code="";
    for(String i:codelist)
    {code+=i+"\r\n";}

    if(path!=NULL_String)
    {
        aLib->RemoveFile(path);
        aLib->WriteTXT(path,code);
    }

    return code;
}

String MINI::toCode(String path) //调用之前需保证树被创建
{
    String code="";
    //根节点一定无名无参数
    for(variable i:parsetree->field)
    {code+=i.first+"="+i.second+"\r\n";}

    for(Tree* i:parsetree->subtree)
    {code+=i->getCode()+"\r\n";}

    this->setCode(code);
    if(path!=NULL_String)
    {
        aLib->RemoveFile(path);
        aLib->WriteTXT(path,code);
    }
    return code;
}

Tree* searchSubTree(vector<Tree*> &nowsubTree,vector<String> &layer,unsigned int nowLayerSub)
{
    for(Tree* i:nowsubTree)
    {
        if(i->name==layer.at(nowLayerSub))
        {
            nowLayerSub++;
            if(nowLayerSub==layer.size()) //已经到达要查找的层级
            {return i;}
            return searchSubTree(i->subtree,layer,nowLayerSub); //未达到，向下一层搜索
        }
    }
    return nullptr;
}

String MINI::readParInTree(vector<String> layer, String par)
{
    Tree *tree=searchSubTree(this->parsetree->subtree,layer,0);
    if(tree==nullptr)
        return NULL_String;
    return tree->readPar(par);
}

String MINI::readVarInTree(vector<String> layer, String var)
{
    Tree *tree=searchSubTree(this->parsetree->subtree,layer,0);
    if(tree==nullptr)
        return NULL_String;
    return tree->readVar(var);
}
