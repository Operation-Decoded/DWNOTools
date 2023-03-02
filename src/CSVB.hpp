#pragma once

#include "parser.hpp"

#include <boost/json.hpp>

#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

enum class DataType : uint32_t
{
    DUMMY        = 0x00,
    STRING       = 0x01,
    INT32        = 0x02,
    FLOAT        = 0x03,
    FLAG         = 0x04,
    INT16        = 0x05,
    INT8         = 0x06,
    VSTRING      = 0x07,
    FX32         = 0x08,
    STRING16     = 0x09,
    DEF_INT32    = 0x0A,
    DEF_FX32     = 0x0B,
    DEF_STRING   = 0x0C,
    VSTRING_UTF  = 0x0D,
    STRING4      = 0x0E,
    STRING8      = 0x0F,
    STRING12     = 0x10,
    DEF_STRING4  = 0x11,
    DEF_STRING8  = 0x12,
    DEF_STRING12 = 0x13,
    DEF_STRING16 = 0x14,
    CHAR4        = 0x15,
    CHAR8        = 0x16,
    DEF_CHAR4    = 0x17,
    DEF_CHAR8    = 0x18,
    HASH32       = 0x19,
    DEF_HASH32   = 0x1A,
    VSTRING_UTF8 = 0x1B,
    DEF_SHORT    = 0x1C
};

struct CSVBHeader
{
    uint32_t magic;
    uint32_t magicVersion;
    uint32_t tableCount;
    uint32_t structureOffset;
    uint32_t stringOffset;
    uint32_t unkOffset1; // vForm
    uint32_t unkOffset2; // vData
    uint32_t unkOffset3; // mappingAddr
};

struct CSVBTable
{
    char name[16];
    uint32_t flag;
    uint32_t entryCount;
    uint32_t entrySize;
    uint32_t fieldCount;
    uint32_t structureOffset;
    uint32_t dataOffset;

public:
    std::string name_str() { return std::string(name); }
};

class CSVBExporter
{
    CSVBHeader header;
    std::vector<CSVBTable> tables;
    std::unique_ptr<char[]> data;
    std::map<std::string, std::vector<DataType>> types;
    std::string fileName;

    boost::json::object structure;
    bool valid;

private:
    bool buildStructure();
    std::string convertType(DataType type, char* ptr);

public:
    CSVBExporter(std::filesystem::path input);
    void writeStructureJSON(std::filesystem::path outPath);
    void write(std::filesystem::path output);

    void setStructure(boost::json::object obj);
    bool isValid();
};

struct StringBlock
{
private:
    std::map<std::string, std::size_t> strings;
    std::size_t lastOffset = 0;

public:
    std::size_t add(std::string string)
    {
        if (strings.contains(string)) return strings[string];

        auto retVal     = lastOffset;
        strings[string] = lastOffset;
        lastOffset += (string.size() + 4) & ~3;

        return retVal;
    }

    std::size_t totalSize() { return lastOffset + (0x10 - (lastOffset % 0x10) % 0x10); }
    auto begin() { return strings.begin(); }
    auto end() { return strings.end(); }
};

struct ImporterEntry
{
    std::string name;
    std::vector<uint32_t> data;
    std::vector<DataType> datatypes;
    CSVBTable table;
};

class CSVBImporter
{
private:
    CSVBHeader header{};
    StringBlock strings;
    std::vector<ImporterEntry> entries;

    uint32_t convertValue(std::string type, std::string value);

public:
    CSVBImporter(std::filesystem::path inputPath);

    void write(std::filesystem::path outputPath);
};

DataType convertToType(std::string type);
std::string getTypeKey(DataType type);
std::string getTypeName(DataType type, int32_t index);
std::size_t getDataTypeSize(DataType type);