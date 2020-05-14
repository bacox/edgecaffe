//
// Created by bacox on 11/05/2020.
//

#include <filesystem>
#include <fstream>
#include <iostream>
#include "../../include/Util/Output.h"

void EdgeCaffe::Output::ensurePathExists(std::string pathToFile)
{
    std::filesystem::path modelFileFS{pathToFile};
    auto parent = modelFileFS.parent_path();
    std::filesystem::create_directories(parent);
}

void EdgeCaffe::Output::toCSV(
        std::string pathToFile, std::vector<std::string> lines, EdgeCaffe::Output::TYPE type, bool writeHeaders
)
{
    ensurePathExists(pathToFile);
    std::ofstream fout(pathToFile, std::ios::out);

    if(writeHeaders)
        fout << headers[type] << std::endl;

    for(const std::string &line : lines)
        fout << line << std::endl;
    fout.close();

    std::cout << "Output written to " << pathToFile << std::endl;
}

void EdgeCaffe::Output::toCSVAppend(std::string pathToFile, std::vector<std::string> lines, EdgeCaffe::Output::TYPE type)
{
    std::ofstream fout;
    if(!fileExists(pathToFile))
    {
        ensurePathExists(pathToFile);
        fout = std::ofstream(pathToFile, std::ios::out);
        fout << headers[type] << std::endl;
    } else
    {
        fout = std::ofstream(pathToFile, std::ios_base::app);
    }

    for(const std::string &line : lines)
        fout << line << std::endl;
    fout.close();

    std::cout << "Output written to " << pathToFile << std::endl;

}

bool EdgeCaffe::Output::fileExists(std::string pathToFile)
{
    return std::filesystem::exists(pathToFile);
}
