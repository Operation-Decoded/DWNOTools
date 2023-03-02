# DWNOTools

A tool to convert file formats from and to Digimon World: Next 0rder.
This currently means only CSVB files without variable data (so no `parameter/scenario` for now).

# Usage

1. Extract the `DigimonDataR.cpk`, for example using CriPakTools.
2. Extract the assets of the `parameter` folder with AssetRipper or similar.
3. Run `DWNOTools.exe -x -i <pathToInputFile> -o <pathToOutputFolder>`

If given a folder it will recursively search for all compatible files.

To rebuild a file run:
1. Run `DWNOTools.exe -p -i <pathToFolder> -o <pathToOutputFile>`


# Building

This project uses CMake in combination [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) for dependency management.

Building the project should be a simple

```
$ git clone git@github.com:Operation-Decoded/BinaryDataConverter.git
$ cd <project dir>
$ cmake .
$ make install
```

## Important Notice
By default CPM.cmake will download all the dependencies, which includes Boost. This can take up to 3 GiB of disk space and take a while.
You can modify and optimize this behavior by configuring CPM environment variables. Please refer to their [documentation](https://github.com/cpm-cmake/CPM.cmake#Options).


# Contact
* Discord: SydMontague#8056, or in either the [Digimon Modding Community](https://discord.gg/cb5AuxU6su) or [Digimon Discord Community](https://discord.gg/0VODO3ww0zghqOCO)
* directly on GitHub
* E-Mail: sydmontague@web.de
* Reddit: [/u/Sydmontague](https://reddit.com/u/sydmontague)