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

String MINI::getBlockName(String &str)
{
    String ret=preprocessor::getToken(str,vector<String>{" ",">"});
    str=str.right(str.length()-ret.length()); //消耗字符
    return ret;
}

String MINI::getParName(String &str)
{
    String ret=preprocessor::getToken(str,vector<String>{" ","="});
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

String MINI::blockBegin(String &str, vector<SI> &stack, vector<String> &layer,unsigned int &nowLayerSub)
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

//直读函数
String MINI::readVar(vector<String> layer, String var)
{
    vector<SI>stack; //栈中包含了是否需要跳过该区块的信息
    unsigned int nowLayerSub=0; //目前匹配到的层级（对应layer和stack的下标）

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
    unsigned int nowLayerSub=0; //目前匹配到的层级（对应layer和stack的下标）

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

String MINI::getBlockCode(vector<String> layer)
{
    vector<SI>stack; //栈中包含了是否需要跳过该区块的信息
    unsigned int nowLayerSub=0; //目前匹配到的层级（对应layer和stack的下标）
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
                    newcode+=oriStr+"\r\n"; //区块结尾不算在内，所以后连接
                }

                if(!blockEnd(str,stack))
                {return NULL_String;}
            }
            else
            {
                //是区块起始
                blockBegin(str,stack,layer,nowLayerSub);
                if(inBlock)
                    newcode+=oriStr+"\r\n"; //区块开头不算在内，所以先连接
                if(stack.size()==layer.size())
                    inBlock=true;
            }
            continue;
        }

        //其它
        if(inBlock)
            newcode+=oriStr+"\r\n";
        continue;
    }
    return NULL_String;
}

String MINI::writeVar(vector<String> layer, String var, String val,String path)
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

            //对区块的处理
            if(preprocessor::removeChar(str,"<"))
            {
                if(preprocessor::removeChar(str,"/")) //检查是区块起始还是区块结尾
                {
                    //是区块结尾
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
                {
                    //是区块起始
                    blockBegin(str,stack,layer,nowLayerSub);
                }
                newcode+=oriStr+"\r\n";
                continue;
            }

            //对field的处理
            if(str.indexOf("=")!=-1)
            {
                if(stack.size()!=0&&stack.back().second) //检查该区块是否需要跳过
                {
                    newcode+=oriStr+"\r\n";
                    continue;
                }

                if(stack.size()!=layer.size()) //检查是否已经到达要查找的区块的层级
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
                    String blockName=blockBegin(str,stack,layer,nowLayerSub);

                    if(stack.size()!=0&&stack.back().second) //检查该区块是否需要跳过
                    {
                        newcode+=oriStr+"\r\n";
                        continue;
                    }

                    if(stack.size()!=layer.size()) //检查是否已经到达要查找的区块的层级
                    {
                        newcode+=oriStr+"\r\n";
                        continue;
                    }

                    //要更改的参数就在这个区块中，先前调用blockBegin已经消耗了区块名和空格，下面直接循环检查参数即可
                    newcode+="<"+blockName+" "; //先连接上区块名
                    newcode+=" "+addcode; //连接上新par，此时已经完成
                    finished=true;
                    if(!preprocessor::isChar(str,">")) //检查该区块是否有参数
                    {
                        while(1) //循环连接区块的其它参数
                        {
                            String parName=getParName(str);
                            preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格

                            if(parName==var) //判断是否是要找的那个par
                            {
                                //已经修改，直接跳过。不过还要消耗字符，否则卡死
                                preprocessor::removeChar(str,"=");
                                preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
                                getParVal(str);
                                preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
                            }
                            else
                            {
                                if(!preprocessor::removeChar(str,"="))
                                {
                                    preprocessor::mistake("每个区块参数必须有一个值");
                                    return NULL_String;
                                }
                                preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
                                String parVal=getParVal(str);
                                preprocessor::removeChar(str," "); //每结束一部分解析都必须去空格
                                newcode+=" "+parName+"="+parVal; //直接连接即可
                            }

                            if(preprocessor::isChar(str,">"))
                                break;

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
    vector<String>stack;
    Tree* nowNode=new Tree(NULL_String); //根节点没名字
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
