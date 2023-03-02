# DWNOTools

A tool to convert file formats from and to Digimon World: Next 0rder.
This currently means only CSVB files without variable data.

# Usage

1. Extract the `DigimonDataR.cpk`, for example using CriPakTools.
2. Extract the assets of the `parameter` folder with AssetRipper or similar.
3. Run `DWNOTools.exe -x -i <pathToInputFile> -o <pathToOutputFolder>`

If given a folder it will recursively search for all compatible files.

To rebuild a file run:
1. Run `DWNOTools.exe -p -i <pathToFolder> -o <pathToOutputFile>`