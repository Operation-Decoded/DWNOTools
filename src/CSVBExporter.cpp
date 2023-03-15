#include "CSVB.hpp"
#include "utils.hpp"

#include <fstream>
#include <iostream>

CSVBExporter::CSVBExporter(std::filesystem::path inputPath)
{
    const auto length = std::filesystem::file_size(inputPath);

    std::ifstream input(inputPath, std::ios::in | std::ios::binary);
    data = std::make_unique<char[]>(length);

    input.read(data.get(), length);

    header = *reinterpret_cast<CSVBHeader*>(data.get());

    if (header.magic != 'BVSC' || header.magicVersion != '3.4v')
    {
        valid = false;
        return;
    }

    for (uint32_t i = 0u; i < header.tableCount; i++)
        tables.push_back(reinterpret_cast<CSVBTable*>(data.get() + sizeof(CSVBHeader))[i]);

    fileName = inputPath.filename().string();

    valid = buildStructure();
    setStructure(getStructureFile(inputPath));
}

bool CSVBExporter::isValid() { return valid; }

void CSVBExporter::hashStrings()
{
    for (auto& entry : tables)
    {
        for (uint32_t i = 0u; i < entry.entryCount; i++)
        {
            char* entryData = data.get() + entry.dataOffset + entry.entrySize * i;

            for (uint32_t i = 0u; i < entry.fieldCount; i++)
            {
                auto type = types[entry.name_str()][i];
                if (type == DataType::VSTRING_UTF8)
                {
                    auto offset = *reinterpret_cast<uint32_t*>(entryData);
                    auto string = std::string(data.get() + header.stringOffset + offset);
                    RainbowTable::addHash(string);
                }
                entryData += static_cast<uint32_t>(getDataTypeSize(type));
            }
        }
    }
}

std::string CSVBExporter::convertType(DataType type, char* ptr)
{
    switch (type)
    {
        case DataType::INT32: return std::to_string(*reinterpret_cast<int32_t*>(ptr));
        case DataType::DEF_INT32: return std::to_string(*reinterpret_cast<int32_t*>(ptr)); // enum?
        case DataType::FLOAT: return std::to_string(*reinterpret_cast<float*>(ptr));
        case DataType::HASH32:
        case DataType::DEF_HASH32:
        {
            uint32_t hash = *reinterpret_cast<uint32_t*>(ptr);
            std::stringstream sstream;
            sstream << std::quoted(RainbowTable::reverseHash(hash).value_or(std::format("{:x}", hash)), '\"', '\"');
            return sstream.str();
        }
        case DataType::VSTRING_UTF8:
        {
            auto offset = *reinterpret_cast<uint32_t*>(ptr);
            auto string = std::string(data.get() + header.stringOffset + offset);
            std::stringstream sstream;
            sstream << std::quoted(string, '\"', '\"');
            return sstream.str();
        }

        default: throw std::invalid_argument("Didn't deal with " + std::to_string((int32_t)type));
    }

    return "INVALID";
}

bool CSVBExporter::buildStructure()
{
    for (auto& entry : tables)
    {
        boost::json::array arr;
        boost::json::object obj;
        std::vector<DataType> typeVec;
        DataType* typeLists = reinterpret_cast<DataType*>(data.get() + header.structureOffset + entry.structureOffset);

        for (uint32_t i = 0u; i < entry.fieldCount; i++)
        {
            boost::json::object a;
            DataType type = typeLists[i];

            a["name"] = getTypeName(type, i);
            a["type"] = getTypeKey(type);
            arr.push_back(a);
            typeVec.push_back(type);
        }

        obj["flag"]                 = entry.flag;
        obj["structure"]            = arr;
        structure[entry.name_str()] = obj;
        types[entry.name_str()]     = typeVec;

        if (entry.flag & 1) return false; // TODO file contains variable CSVB, unsupported
    }

    return true;
}

void CSVBExporter::writeStructureJSON(std::filesystem::path outPath)
{
    if (!isValid()) return;

    std::filesystem::create_directories(outPath.parent_path());

    std::ofstream jsonOutput(outPath);
    pretty_print(jsonOutput, structure);
}

void CSVBExporter::write(std::filesystem::path outPath)
{
    if (!isValid()) return;

    if (!std::filesystem::exists(outPath))
    {
        if (outPath.has_parent_path()) std::filesystem::create_directories(outPath);
    }
    else if (!std::filesystem::is_directory(outPath))
        throw std::invalid_argument("Error: target path is not a directory.");

    for (auto& entry : tables)
    {
        std::filesystem::path path = (outPath / fileName / entry.name_str()).concat(".csv");
        if (path.has_parent_path()) std::filesystem::create_directories(path.parent_path());

        std::ofstream output(path);

        bool first = true;
        for (uint32_t i = 0u; i < entry.fieldCount; i++)
        {
            if (first)
                first = false;
            else
                output << ",";
            output << structure[entry.name_str()].as_object()["structure"].as_array()[i].as_object()["name"];
        }
        output << std::endl;

        for (uint32_t i = 0u; i < entry.entryCount; i++)
        {
            char* entryData = data.get() + entry.dataOffset + entry.entrySize * i;

            bool first2 = true;
            for (uint32_t i = 0u; i < entry.fieldCount; i++)
            {
                if (first2)
                    first2 = false;
                else
                    output << ",";

                output << convertType(types[entry.name_str()][i], entryData);
                entryData += static_cast<uint32_t>(getDataTypeSize(types[entry.name_str()][i]));
            }
            output << std::endl;
        }
    }

    writeStructureJSON((getRawStructuresPath() / fileName).concat(".json"));
}

void CSVBExporter::setStructure(boost::json::object obj)
{
    if (obj.size() != 0) structure = obj;
}
