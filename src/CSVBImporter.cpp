#include "CSVB.hpp"
#include "utils.hpp"

#include <parser.hpp>

CSVBImporter::CSVBImporter(std::filesystem::path inputPath)
{
    if (!std::filesystem::exists(inputPath)) throw std::invalid_argument("Error: input path does not exist.");
    if (!std::filesystem::is_directory(inputPath)) throw std::invalid_argument("Error: input path is not a directory.");

    auto structure = getStructureFile(inputPath, true);

    strings.add("");

    for (auto& table : structure)
    {
        ImporterEntry entry{};
        std::string name        = table.key();
        boost::json::object obj = table.value().as_object();
        boost::json::array arr  = obj["structure"].as_array();
        entry.name              = name;
        entry.table.flag        = static_cast<uint32_t>(obj["flag"].as_int64());

        std::copy(name.begin(), name.end(), std::begin(entry.table.name));

        // data types / structure
        for (auto& dType : arr)
        {
            std::string str(dType.as_object()["type"].as_string());
            DataType type = convertToType(str);
            entry.datatypes.push_back(type);
            entry.table.entrySize += static_cast<uint32_t>(getDataTypeSize(type));
            entry.table.fieldCount++;
        }

        // read csv data
        {
            std::ifstream ifstr((inputPath / name).concat(".csv"));
            aria::csv::CsvParser parser(ifstr);

            auto& entryStruct = arr;
            auto rowId        = -1;
            for (auto& row : parser)
            {
                // skip header
                if (++rowId == 0) continue;

                auto colId = 0;

                for (auto& col : row)
                {
                    auto& obj = entryStruct[colId++].as_object();
                    std::string type(obj["type"].as_string());

                    entry.data.push_back(convertValue(type, col));
                }
            }

            entry.table.entryCount = rowId;
        }

        entries.push_back(entry);
    }
}

uint32_t CSVBImporter::convertValue(std::string type, std::string value)
{
    DataType dataType = convertToType(type);

    switch (dataType)
    {
        case DataType::DEF_INT32:
        case DataType::INT32:
        {
            auto val1 = std::stoi(value.c_str());
            return *reinterpret_cast<uint32_t*>(&val1);
        }
        case DataType::FLOAT:
        {
            auto val2 = std::stof(value.c_str());
            return *reinterpret_cast<uint32_t*>(&val2);
        }
        case DataType::HASH32:
        case DataType::DEF_HASH32:
        {
            return std::stoul(value.c_str(), nullptr, 16);
        }
        case DataType::VSTRING_UTF8:
        {
            auto offset = strings.add(value);
            return *reinterpret_cast<uint32_t*>(&offset);
        }
    }

    return 0;
}

void CSVBImporter::write(std::filesystem::path outputPath)
{
    if (!std::filesystem::exists(outputPath))
    {
        if (outputPath.has_parent_path()) std::filesystem::create_directories(outputPath.parent_path());
    }
    else if (!std::filesystem::is_regular_file(outputPath))
        throw std::invalid_argument("Error: target path is not a file.");

    const std::size_t headerSize = sizeof(CSVBHeader) + sizeof(CSVBTable) * entries.size();

    BinaryWriteBuffer dataBuff;
    BinaryWriteBuffer structBuff;
    BinaryWriteBuffer raw;

    std::ofstream out(outputPath, std::ios::out | std::ios::binary);

    header.magic        = 'BVSC';
    header.magicVersion = '3.4v';
    header.tableCount   = static_cast<uint32_t>(entries.size());

    // write data section
    for (auto& entry : entries)
    {
        entry.table.dataOffset = static_cast<uint32_t>(headerSize + dataBuff.size());
        dataBuff.write(entry.data);
    }
    int32_t toPad = (0x10 - ((headerSize + dataBuff.size()) % 0x10)) % 0x10;
    for (int32_t i = 0; i < toPad; i++)
        dataBuff.write((uint8_t)0);

    header.structureOffset = static_cast<uint32_t>(headerSize + dataBuff.size());

    // write structure section
    for (auto& entry : entries)
    {
        entry.table.structureOffset = static_cast<uint32_t>(structBuff.size());
        structBuff.write(entry.datatypes);
    }
    structBuff.padTo(0x10);
    header.stringOffset = static_cast<uint32_t>(headerSize + dataBuff.size() + structBuff.size());

    // write string section
    std::vector<uint8_t> stringRaw;
    stringRaw.resize(strings.totalSize());

    for (auto& entry : strings)
        memcpy(stringRaw.data() + entry.second, entry.first.data(), entry.first.size());

    // finalize header
    header.unkOffset1 = static_cast<uint32_t>(headerSize + dataBuff.size() + structBuff.size() + stringRaw.size());
    header.unkOffset2 = static_cast<uint32_t>(headerSize + dataBuff.size() + structBuff.size() + stringRaw.size());
    header.unkOffset3 = 0u;

    // write everything to file
    raw.write(header);
    for (auto& entry : entries)
        raw.write(entry.table);
    raw.write(dataBuff);
    raw.write(structBuff);
    raw.write(stringRaw);

    out.write(reinterpret_cast<char*>(raw.data.data()), raw.data.size());
}
