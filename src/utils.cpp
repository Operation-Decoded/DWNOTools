#include "utils.hpp"

#include <fstream>
#include <regex>

// taken from Boost.JSON documentation
// https://www.boost.org/doc/libs/1_80_0/libs/json/doc/html/json/examples.html#json.examples.pretty
// Licensed under Boost Software License, Version 1.0
// see THIRD-PARTY-NOTICE file for full license text
void pretty_print(std::ostream& os, boost::json::value const& jv, std::string* indent)
{
    constexpr int32_t INDENT_COUNT = 2;

    std::string indent_;
    if (!indent) indent = &indent_;
    switch (jv.kind())
    {
        case boost::json::kind::object:
        {
            os << "{\n";
            indent->append(INDENT_COUNT, ' ');
            auto const& obj = jv.get_object();
            if (!obj.empty())
            {
                auto it = obj.begin();
                for (;;)
                {
                    os << *indent << boost::json::serialize(it->key()) << " : ";
                    pretty_print(os, it->value(), indent);
                    if (++it == obj.end()) break;
                    os << ",\n";
                }
            }
            os << "\n";
            indent->resize(indent->size() - INDENT_COUNT);
            os << *indent << "}";
            break;
        }

        case boost::json::kind::array:
        {
            os << "[\n";
            indent->append(INDENT_COUNT, ' ');
            auto const& arr = jv.get_array();
            if (!arr.empty())
            {
                auto it = arr.begin();
                for (;;)
                {
                    os << *indent;
                    pretty_print(os, *it, indent);
                    if (++it == arr.end()) break;
                    os << ",\n";
                }
            }
            os << "\n";
            indent->resize(indent->size() - INDENT_COUNT);
            os << *indent << "]";
            break;
        }

        case boost::json::kind::string:
        {
            os << boost::json::serialize(jv.get_string());
            break;
        }

        case boost::json::kind::uint64: os << jv.get_uint64(); break;

        case boost::json::kind::int64: os << jv.get_int64(); break;

        case boost::json::kind::double_: os << jv.get_double(); break;

        case boost::json::kind::bool_:
            if (jv.get_bool())
                os << "true";
            else
                os << "false";
            break;

        case boost::json::kind::null: os << "null"; break;
    }

    if (indent->empty()) os << "\n";
}

std::string getFileAsString(std::filesystem::path path)
{
    if (!std::filesystem::exists(path)) return "";

    std::ifstream structFile(path);
    std::stringstream contents;
    contents << structFile.rdbuf();
    return contents.str();
}

boost::json::object getStructureFile(std::filesystem::path source, bool useRaw)
{
    auto structJson               = getFileAsString(getStructuresPath() / "structures.json");
    boost::json::object structure = boost::json::parse(structJson).as_object();

    for (auto& var : structure)
    {
        if (std::regex_search(source.string(), std::regex{ var.key_c_str() }))
        {
            std::filesystem::path formatPath = getStructuresPath() / std::string(var.value().as_string());
            return boost::json::parse(getFileAsString(formatPath)).as_object();
        }
    }

    if (useRaw)
    {
        auto folderName               = source.parent_path().filename();
        std::filesystem::path rawPath = (getRawStructuresPath() / folderName).concat(".json");
        if (std::filesystem::is_regular_file(rawPath)) return boost::json::parse(getFileAsString(rawPath)).as_object();
    }

    return {};
}

std::filesystem::path getStructuresPath() { return "structures"; }
std::filesystem::path getRawStructuresPath() { return getStructuresPath() / "raw"; }