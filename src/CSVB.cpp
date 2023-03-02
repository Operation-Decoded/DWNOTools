#include "CSVB.hpp"

#include <format>

class DataTypeWrapper
{
private:
    using get_size     = std::function<std::size_t()>;
    using to_type_name = std::function<std::string(int32_t)>;

    struct DataTypeImpl
    {
        DataType type = DataType::DUMMY;
        std::string name;
        std::size_t size = 0;
    };

    std::map<DataType, DataTypeImpl> impl;
    std::map<std::string, DataType> nameToType;

private:
    DataTypeWrapper()
    {
        add(DataType::INT32, "int", 4);
        add(DataType::DEF_INT32, "def_int", 4);
        add(DataType::FLOAT, "float", 4);
        add(DataType::HASH32, "hash", 4);
        add(DataType::DEF_HASH32, "def_hash", 4);
        add(DataType::VSTRING_UTF8, "vstring8", 4);
    }

    void add(DataType type, std::string name, std::size_t size)
    {
        nameToType.try_emplace(name, type);
        impl.try_emplace(type, type, name, size);
    }

public:
    static DataTypeWrapper instance;
    DataTypeImpl getImpl(DataType type)
    {
        if (!impl.contains(type))
            throw std::invalid_argument(std::format("Tried using undefined DataType: {}", static_cast<uint32_t>(type)));
        return impl[type];
    }
    DataTypeImpl getImpl(std::string name) { return getImpl(nameToType[name]); }
};

DataTypeWrapper DataTypeWrapper::instance;

std::string getTypeKey(DataType type) { return DataTypeWrapper::instance.getImpl(type).name; }

std::size_t getDataTypeSize(DataType type) { return DataTypeWrapper::instance.getImpl(type).size; }

DataType convertToType(std::string type) { return DataTypeWrapper::instance.getImpl(type).type; }

std::string getTypeName(DataType type, int32_t index)
{
    return std::format("unk_{}_{}", getTypeKey(type), std::to_string(index));
}