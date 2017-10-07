#pragma once
#include "Lib.h"
#include <vector>
#include <list>
#include <utility>

typedef pair<String,String> variable;
typedef pair<String,bool>SI; //包含是否跳过信息的栈元素类型

class Tree
{
private:
    Tree* father;
public:
    //基本操作
    void addField(variable var);
    void deleteField(String var);
    void addPar(variable var);
    void deletePar(String var);
    void addChildNode(Tree *tree);
    void deleteTree(Tree *tree);
    Tree* getFather();
    static Tree* searchSubTree(Tree *nowTree, vector<String> &layer);
    //实际操作
    String readVar(String var);
    String readPar(String var);
    //特殊操作
    String getCode(String tab=NULL_String);
    //子树和成员
    vector<Tree*>subtree;
    vector<variable>field;
    vector<variable>par;
    String name;

    Tree(String name,Tree* father=nullptr):father(father),name(name){}
    ~Tree();
};

enum MINIsta{CODE,TREE,ALL};
class MINI
{
public:
    MINI(String content,bool ispath=true);
    MINI(Tree *tree);
    ~MINI();

    void setCode(String code);
    Tree* toTree(); //把代码翻译成树
    Tree* getTree();
    String toCode(String path=NULL_String);
    String getCode(String path=NULL_String);
    MINIsta getState();
    void format();
    //读
    String readVar(vector<String>layer,String var);
    String getBlockCode(vector<String>layer);
    String readPar(vector<String>layer,String par);
    String readVarInTree(vector<String>layer,String var);
    String readParInTree(vector<String>layer,String par);
    //写
    String writeVar(vector<String>layer, String var, String val, String path=NULL_String);
    String writePar(vector<String>layer, String var, String val, String path=NULL_String);

private:
    void initParsetree();
    list<String>codelist;
    Tree *parsetree=nullptr;
};
