#pragma once
#include "Lib.h"
#include <vector>
#include <list>
#include <utility>

typedef pair<String,String> variable;

class Tree
{
private:
    vector<Tree*>subtree;
    vector<variable>field;
    vector<variable>par;
public:
    //基本操作
    void addField(String var,String val);
    void deleteField(String var);
    void addPar(String var,String val);
    void deletePar(String var);
    void addTree(Tree *tree);
    void deleteTree(Tree *tree);
    //实际操作
    String readVar(String var);
    String readPar(String var);
    //至于遍历子树就自己遍历吧……
};

class MINI
{
public:
    MINI(String path);
    MINI(){}
    void setCode(String code);
    void toTree();
    //读
    String readVar(vector<String>layer,String var);
    String getCode(vector<String>layer);
    bool haveVar(vector<String>layer,String var);
    bool havelayer(vector<String>layer);
    //写
    void writeVar(vector<String>layer,String var,String val);

private:
    //分析有关
    String getBlockName(String &str);
    //存储有关
    list<String>codelist;
    Tree *parsetree=nullptr;
    bool isParse();
};
