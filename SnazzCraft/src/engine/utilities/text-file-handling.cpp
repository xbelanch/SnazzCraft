#include "snazzcraft-engine/utilities/text-file-handling.hpp"

void SnazzCraft::ParseData(std::string& Extract, std::string Data, unsigned int& Index, char* EndChar)
{
    auto ShouldBreak = [&Data, &Index, EndChar]() -> bool
    {
        if (Index >= Data.size()) return true;

        if (EndChar != NULL) {
            if (Data[Index] == *EndChar) return true;
        }

        return false;
    };

    while (!ShouldBreak())
    {
        Extract += Data[Index];
        Index++;
    }
}