#pragma once

#include <boost/json.hpp>

#include <filesystem>
#include <ranges>
#include <type_traits>
#include <vector>

template<typename T>
concept trivial_copyable = std::is_trivially_copyable<T>::value;

class BinaryWriteBuffer
{
public:
    std::vector<uint8_t> data;

public:
    template<typename T>
    requires(std::ranges::range<T>) void write(T& val)
    {
        for (auto& v : val)
            write(v);
    }

    template<typename T>
    requires(trivial_copyable<T>) void write(T& val)
    {
        const uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
        std::copy(ptr, ptr + sizeof(T), std::back_inserter(data));
    }

    void write(uint8_t val) { data.push_back(val); }

    void write(BinaryWriteBuffer val) { std::copy(val.data.begin(), val.data.end(), std::back_inserter(data)); }

    void padTo(std::size_t pad)
    {
        auto size       = data.size();
        auto paddedSize = size + (pad - (size % pad)) % pad;
        data.resize(paddedSize);
    }

    std::size_t size() { return data.size(); }
};

void pretty_print(std::ostream& os, boost::json::value const& jv, std::string* indent = nullptr);
std::string getFileAsString(std::filesystem::path path);
boost::json::object getStructureFile(std::filesystem::path source, bool useRaw = false);
std::filesystem::path getStructuresPath();
std::filesystem::path getRawStructuresPath();