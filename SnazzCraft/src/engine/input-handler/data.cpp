#include "snazzcraft-engine/input-handler/data.hpp"

SnazzCraft::Data::~Data()
{
    for (unsigned int I = 0; I < this->Items.size(); I++) { 
        switch (this->Types[I]) {
            case SNAZZCRAFT_DATA_TYPE_STD_STRING:
                delete static_cast<std::string*>(this->Items[I]);
                break;

            case SNAZZCRAFT_DATA_TYPE_KEY:
                delete static_cast<unsigned char*>(this->Items[I]);
                break;

            case SNAZZCRAFT_DATA_TYPE_DVEC2:
                delete static_cast<glm::dvec2*>(this->Items[I]);
                break;

            case SNAZZCRAFT_DATA_TYPE_SWITCH_MODE:
                delete static_cast<unsigned char*>(this->Items[I]);
                break;

            case SNAZZCRAFT_DATA_TYPE_INT:
                delete static_cast<int*>(this->Items[I]);
                break;

            case SNAZZCRAFT_DATA_TYPE_BOOL:
                delete static_cast<bool*>(this->Items[I]);
                break;

            default:
                break;
        }
    }
}

void* SnazzCraft::Data::AccessDataType(unsigned char Type)
{
    for (unsigned int I = 0; I < this->Types.size(); I++) {
        if (this->Types[I] == Type) {
            return this->Items[I];
        }
    }

    return nullptr;
}