#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <iostream>

#include "ConsoleColor.h"
#include "MyPaths .h"
#include "Gameplay.h"

class FileStoredGamePlay : public Gameplay {
public:
    FileStoredGamePlay(std::string filename, std::string gameName = "Stored File Game");
    std::string gamename() { return gameName; }
    bool begin();
    int run();
protected:
    bool FileReadCsv();
    bool FileReadTxt();
private:
    std::vector<float> Timelines;
    std::vector<unsigned int> Bitsets;
    std::string gameName, filename;
    int curCycle;
};
FileStoredGamePlay::FileStoredGamePlay(std::string filename, std::string gameName) {
    this->filename = filename;
    this->gameName = gameName;
}
bool FileStoredGamePlay::begin() {
    curCycle = 0;
    Bitsets.clear();
    Timelines.clear();
    Gameplay::begin();
    return FileReadTxt();
}
int FileStoredGamePlay::run()
{
    int numCycles = Bitsets.size();
    if (curCycle < numCycles) {
        int bitset = Bitsets.at(curCycle);
        int patternSize = 1000 + bitset; // distinct shift by 1000

        curCycle++;
        std::cout << white << curCycle << " of " << numCycles << ".  ";
        return Gameplay::play(patternSize);
    }
    else {
        ShowResults();
        return 2001;
    }
}
bool FileStoredGamePlay::FileReadCsv() {
    // fills Timelines & Bitsets
    std::ifstream ifs;
    ifs.open(filename);
    if (ifs.is_open()) {
        std::cout << "csv file opened" << std::endl;
        float timeline;
        unsigned int bitset;

        std::vector<std::vector<std::string>> content;
        std::vector<std::string> row;
        std::string line, word;

        while (getline(ifs, line))
        {
            row.clear();
            std::stringstream str(line);
            while (getline(str, word, ';')) {
                row.push_back(word);
            }
            content.push_back(row);
        }

        ifs.close();
        for (int i = 0;i < content.size();i++)
        {
            timeline = std::stof(content[i][0]);
            bitset = std::stof(content[i][1]);
            Timelines.push_back(timeline);
            Bitsets.push_back(bitset);
            //for (int j = 0;j < content[i].size();j++)
            //    std::cout << content[i][j] << "\t ";
            //std::cout << "\n";
        }
        if (Timelines.size() != Bitsets.size()) {
            std::cout << red << " file is corrupted" << std::endl;
            return false;
        }
        std::cout << "time" << "\t " << "patt" << "\n";
        for (int i = 0; i < Timelines.size(); i++)
        {
            std::cout << Timelines.at(i) << "\t " << Bitsets.at(i) << std::endl;
        }
        return true;
    }
    else {
        std::cout << red << "can't open file " << std::endl;
        return false;
    }
}

bool FileStoredGamePlay::FileReadTxt()
{
    // fills  Bitsets only
    std::ifstream ifs;
    ifs.open(filename);
    if (ifs.is_open()) {
        std::cout << "txt file opened " << gameName << std::endl;
        int bitset;
        std::string line;

        while (getline(ifs, line))
        {
            bitset = atoi(line.c_str());
            Bitsets.push_back(bitset);
        }
        ifs.close();

        if (Bitsets.size() == 0) {
            std::cout << red << " file is empty " << gameName << std::endl;
            return false;
        }
        std::cout << "patt" << "\n";
        for (int i = 0; i < Bitsets.size(); i++)
        {
            std::cout << Bitsets.at(i) << std::endl;
        }
        return true;
    }
    else {
        std::cout << red << "can't open file " << gameName << std::endl;
        return false;
    }
}