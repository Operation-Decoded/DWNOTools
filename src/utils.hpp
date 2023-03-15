#pragma once

#include <boost/json.hpp>

#include <filesystem>
#include <map>
#include <optional>
#include <ranges>
#include <set>
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

uint32_t makeHash(const std::string& input);

class RainbowTable
{
    static RainbowTable instance;

    std::map<uint32_t, std::string> table;
    std::set<uint32_t> usedHashes;

private:
    void generate(const std::string& fmt, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto str = std::vformat(fmt, std::make_format_args(i));
            addHash(str);
        }
    }

    RainbowTable()
    {
        _addHash("");
        _addHash("0");
        _addHash("kara");

        // model names
        generate("a{:03}", 1000);
        generate("b{:03}", 1000);
        generate("c{:03}", 1000);
        generate("d{:03}", 1000);
        generate("e{:03}", 1000);
        generate("f{:03}", 1000);
        generate("g{:03}", 1000);
        generate("k{:03}", 1000);
        generate("z{:03}", 1000);

        // item names
        generate("item_other_{:03}", 1000);
        generate("item_food_{:03}", 1000);
        generate("item_recover_{:03}", 1000);
        generate("item_battle_{:03}", 1000);
        generate("item_keyitem_{:03}", 1000);
        generate("item_material_{:03}", 1000);

        generate("icon{}", 10000);
        generate("mailCode{}", 10000);
        // other
        generate("training_{:03}", 1000);
        generate("card_{:03}", 1000);

        generate("digi_top_day_week_{:01}", 10);
        generate("digi_top_week_{:01}", 10);
        generate("digi_top_season_{:01}", 10);
        generate("item_food_lineage_{:02}", 100);

        generate("flagset_{}", 1000);
        generate("emotion_{:03}", 1000);
        generate("TWN_EXCL_{:01}", 10);

        generate("grade_up_{:03}", 1000);

        generate("CARE_STEALTH_{:01}", 10);
        generate("CARE_TRAINING_UP_{:01}", 10);
        generate("EDUCATION_EVOLUTION_INFO_{:01}", 10);
        _addHash("partner_weight");
        _addHash("evo_dojo_upbringing_miss");
        _addHash("evo_dojo_bonds");
        _addHash("evo_dojo_upbringing");
        _addHash("evo_dojo_win");
        _addHash("evo_dojo_key_digimon");
        _addHash("evo_dojo_necessary");
        _addHash("evo_dojo_empty");
        _addHash("PLAYR");
        _addHash("HIMAR");
        _addHash("MAMEO");
        _addHash("MIREI");
        _addHash("JIJIM");
        _addHash("YUKI0");
        _addHash("YUKIA");
        _addHash("YUKIB");
        _addHash("YUKIC");
        _addHash("YUKID");
        _addHash("RIKA0");
        _addHash("RIKAA");
        _addHash("RIKAB");
        _addHash("RIKAC");
        _addHash("RIKAD");

        // skills
        _addHash("non_00");
        _addHash("FIR_A");
        _addHash("FIR_B");
        _addHash("FIR_C");
        _addHash("FIR_D");
        _addHash("FIR_E");
        _addHash("FIR_F");
        _addHash("FIR_G");
        _addHash("ICE_A");
        _addHash("ICE_B");
        _addHash("ICE_C");
        _addHash("ICE_D");
        _addHash("ICE_E");
        _addHash("ICE_F");
        _addHash("ICE_G");
        _addHash("ELC_A");
        _addHash("ELC_B");
        _addHash("ELC_C");
        _addHash("ELC_D");
        _addHash("ELC_E");
        _addHash("ELC_F");
        _addHash("ELC_G");
        _addHash("PIS_A");
        _addHash("PIS_B");
        _addHash("PIS_C");
        _addHash("PIS_D");
        _addHash("PIS_E");
        _addHash("PIS_F");
        _addHash("PIS_G");
        _addHash("DRK_A");
        _addHash("DRK_B");
        _addHash("DRK_C");
        _addHash("DRK_D");
        _addHash("DRK_E");
        _addHash("DRK_F");
        _addHash("DRK_G");
        _addHash("LIT_A");
        _addHash("LIT_B");
        _addHash("LIT_C");
        _addHash("LIT_D");
        _addHash("LIT_E");
        _addHash("LIT_F");
        _addHash("LIT_G");
        _addHash("CMB_A");
        _addHash("CMB_B");
        _addHash("CMB_C");
        _addHash("CMB_D");
        _addHash("CMB_E");
        _addHash("CMB_F");
        _addHash("CMB_G");
        _addHash("MEC_A");
        _addHash("MEC_B");
        _addHash("MEC_C");
        _addHash("MEC_D");
        _addHash("MEC_E");
        _addHash("MEC_F");
        _addHash("MEC_G");
        _addHash("GRB_A");
        _addHash("GRB_B");
        _addHash("GRB_C");
        _addHash("GRB_D");
        _addHash("GRB_E");
        _addHash("GRB_F");
        _addHash("GRB_G");

    };
    RainbowTable(RainbowTable& copy) = delete;

public:
    void _addHash(const std::string& input)
    {
        auto hash = makeHash(input);
        if (!table.contains(hash) || table[hash].length() > input.length()) table[makeHash(input)] = input;
    }
    bool _hasHash(uint32_t input) { return table.contains(input); }
    std::optional<std::string> _reverseHash(uint32_t input)
    {
        if (!_hasHash(input)) return {};

        usedHashes.insert(input); //.emplace(input);
        return table[input];
    }
    boost::json::object _toJSON()
    {
        boost::json::object obj;

        std::vector<std::pair<std::string, std::string>> vec;

        for (auto val : usedHashes)
            vec.emplace_back(table[val], std::format("{:x}", val));

        std::sort(vec.begin(), vec.end());

        for (auto val : vec)
            obj[val.first] = val.second;

        return obj;
    }

public:
    static RainbowTable& getInstance() { return instance; }
    static void addHash(const std::string& input) { getInstance()._addHash(input); }
    static bool hasHash(uint32_t input) { return getInstance()._hasHash(input); }
    static std::optional<std::string> reverseHash(uint32_t input) { return getInstance()._reverseHash(input); }
    static boost::json::object toJSON() { return getInstance()._toJSON(); }
};

void pretty_print(std::ostream& os, boost::json::value const& jv, std::string* indent = nullptr);
std::string getFileAsString(std::filesystem::path path);
boost::json::object getStructureFile(std::filesystem::path source, bool useRaw = false);
std::filesystem::path getStructuresPath();
std::filesystem::path getRawStructuresPath();