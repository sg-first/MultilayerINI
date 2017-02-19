#pragma once
#include "Lib.h"

class preprocessor
{
public:
    static void deleteSpace(String &str); //把几个连一起的空格变成一个空格
    static void removeChar(String &str,String Char);
    static bool isChar(String &str,String Char);
};
