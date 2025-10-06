#include "open_flight.h"

ushort byteSwap(ushort value){ return (value << 8) | (value >> 8); }

short byteSwap(short value){ return (value << 8) | ((value >> 8)& 0x00FF); }

int byteSwap(int value)
{ 
    int hh = value << 24;
    int hl = (value << 8) & 0x00FF0000;
    int lh = (value >> 8) & 0x0000FF00;
    int ll = (value >> 24) & 0x000000FF;

    return hh | hl | lh | ll;
}

double byteSwap(double value)
{ 
    double ret;

    char *dst = reinterpret_cast<char*>(&ret), 
         *src = reinterpret_cast<char*>(&value);

    #pragma unroll(4)
    for(int i = 0; i != sizeof(double); ++i){
        dst[i] = src[sizeof(double) - 1 - i];
    }
    return ret;
}



std::variant<
    std::unique_ptr<open_flight::record::Header>, 
    std::unique_ptr<open_flight::record::Group>, 
    std::unique_ptr<open_flight::record::Object>, 
    std::unique_ptr<open_flight::record::Face>, 
    std::monostate>
open_flight::Parser::factoryRecords(short opcode, ushort size, std::ifstream& stream)
{
    switch (opcode){
    case record::OPCODE_ID::GROUP:{
        auto ptr = std::make_unique<record::Group>();
        ptr->opcode = opcode;
        ptr->length = size;
        stream.read((char*)ptr.get() + 4, size - 4);
        return ptr;
        break; 
    }
    case record::OPCODE_ID::OBJECT:{
        auto ptr = std::make_unique<record::Object>();
        ptr->opcode = opcode;
        ptr->length = size;
        stream.read((char*)ptr.get() + 4, size - 4);
        return ptr;
        break; 
    }
    case record::OPCODE_ID::FACE:{
        auto ptr = std::make_unique<record::Face>();
        ptr->opcode = opcode;
        ptr->length = size;
        stream.read((char*)ptr.get() + 4, size - 4);
        return ptr;
        break; 
    }
    case record::OPCODE_ID::HEADER:{
        auto ptr = std::make_unique<record::Header>();
        ptr->opcode = opcode;
        ptr->length = size;
        stream.read((char*)ptr.get() + 4, size - 4);
        return ptr;
        break; 
    }
    default: 
        return std::monostate{}; 
    }
    return std::monostate{};            
}    

void open_flight::Parser::print_info::operator()(std::monostate& ptr)
{
    return;
}
        
void open_flight::Parser::print_info::operator()(std::unique_ptr<open_flight::record::Face>& ptr)
{
    std::cout <<level_<< "ID: " << ptr->id << ", "
              << "Color Name Index: " << ptr->colorNameIndex << ", "
              << "Material Index: "  << ptr->materialIndex 
              << std::endl;
}

void 
open_flight::Parser::print_info::operator()(std::unique_ptr<open_flight::record::Object>& ptr)
{
    std::cout << level_ << "ID: " << ptr->id << std::endl;
}

void open_flight::Parser::print_info::operator()(std::unique_ptr<open_flight::record::Group>& ptr)
{
    std::cout << level_ << "ID: " << ptr->id << std::endl;
}

void open_flight::Parser::print_info::operator()(std::unique_ptr<open_flight::record::Header>& ptr)
{
    std::cout << level_ << "ID: " << ptr->id << std::endl;
}

 bool open_flight::Parser::isOpcode(short opcode, short size)
{
    switch (opcode)
    {
    case record::OPCODE_ID::GROUP:
        if(size == sizeof(record::Group)) return true;
        break;
    case record::OPCODE_ID::OBJECT:
        if(size == sizeof(record::Object)) return true;
        break;
    case record::OPCODE_ID::FACE:
        if(size == sizeof(record::Face)) return true;
        break;
    case record::OPCODE_ID::HEADER:
        if(size == sizeof(record::Header) - 4) return true;
        break;
    case 11:
        if(size == 4) return true;
        break;
    case 10:
        if(size == 4) return true;
        break;
    default:
        return false;
        break;
    }
    return false;
}       

 