#pragma once

#include <fstream>
#include <string>
#include <iostream>

namespace SnazzCraft
{
    void ParseData(std::string& Extract, std::string Data, unsigned int& Index, char* EndChar); // WARNING: Index may end on EndChar, causing next call to fail emmediatly
}



