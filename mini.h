#pragma once
#include "Lib.h"
#include <vector>
#include <list>

class MINI
{
public:
    MINI(String path);
    MINI(){}
    void setCode(String code);
    //读
    String readVar(vector<String>layout,String var);
    String getCode(vector<String>layout);
    bool haveVar(vector<String>layout,String var);
    bool haveLayout(vector<String>layout);
    //写
    void writeVar(vector<String>layout,String var,String val);

private:
    list<String>codelist;
};
