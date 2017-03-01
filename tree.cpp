#include "mini.h"

void Tree::addField(String var, String val)
{this->field.push_back(variable(var,val));}

void Tree::addTree(Tree *tree)
{this->subtree.push_back(tree);}

void Tree::deleteField(String var)
{
    for(auto itor=field.begin();itor!=field.end();)
    {
        if((*itor).first==var)
        {
            /*itor=*/field.erase(itor);
            return;
        }
        else
        {itor++;}
    }
}

void Tree::deleteTree(Tree *tree)
{
    for(auto itor=subtree.begin();itor!=subtree.end();)
    {
        if(*itor==tree)
        {
            /*itor=*/subtree.erase(itor);
            return;
        }
        else
        {itor++;}
    }
}

String Tree::readVar(String var)
{
    for(variable i:field)
    {
        if(i.first==var)
            return i.second;
    }
    return NULL_String;
}
