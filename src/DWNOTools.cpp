#include "CSVB.hpp"
#include "utils.hpp"

#include <boost/program_options.hpp>

#include <filesystem>
#include <iostream>
#include <string>

/* Hashing algorithm
public static uint makeHash(string hashString)
    {
        if (hashString == null || hashString.Length <= 0)
        {
            return uint.MaxValue;
        }
        byte[] bytes = Encoding.ASCII.GetBytes(hashString);
        uint num = 2166136261u;
        for (int i = 0; i < bytes.Length; i++)
        {
            num *= 16777619;
            num ^= bytes[i];
        }
        return num;
    }
*/

int main(int count, char* args[])
{
    namespace po = boost::program_options;

    po::variables_map vm;

    try
    {
        po::options_description desc("Usage: binaryconverter.exe <structurePath> [options]\n\nAllowed Options");

        auto options = desc.add_options();
        options("help,h", "This text.");
        options("input,i", po::value<std::string>(), "Input file or folder. Required");
        options("output,o", po::value<std::string>(), "Output file or folder. Required");
        options(
            "pack,p",
            "Build a CSVB file out of the given input folder. The folder name must correspond to a valid structure.");
        options("extract,x",
                "Extract a CSVB out of a given file."
                "A raw structure will be created in /structures/raw/, which is necessary for rebuilding."
                "If a folder is given it will be recursively search for CSVB files in it.");

        po::store(po::command_line_parser(count, args).options(desc).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 1;
        }

        if (!vm.count("input"))
        {
            std::cout << "You must specify an input path." << std::endl;
            std::cout << desc << std::endl;
            return 1;
        }
        if (!vm.count("output"))
        {
            std::cout << "You must specify an output path." << std::endl;
            std::cout << desc << std::endl;
            return 1;
        }
        if ((vm.count("pack") + vm.count("extract")) != 1)
        {
            std::cout << "You must specify either --pack or --extract." << std::endl;
            std::cout << desc << std::endl;
            return 1;
        }

        std::filesystem::path input  = vm["input"].as<std::string>();
        std::filesystem::path output = vm["output"].as<std::string>();

        if (vm.count("pack"))
        {
            CSVBImporter importer(input);
            importer.write(output);
            return 0;
        }

        if (vm.count("extract"))
        {
            if (std::filesystem::is_directory(input))
            {
                std::filesystem::recursive_directory_iterator itr(input);

                for (auto& path : itr)
                {
                    if (!path.is_regular_file()) continue;

                    std::cout << path << std::endl;
                    std::cout << output / std::filesystem::relative(path.path().parent_path(), input) << std::endl;

                    CSVBExporter exporter(path);
                    if (exporter.isValid())
                        exporter.write(output / std::filesystem::relative(path.path().parent_path(), input));
                }
            }
            else if (std::filesystem::is_regular_file(input))
            {
                CSVBExporter exporter(input);
                if (exporter.isValid()) exporter.write(output);
            }
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}
