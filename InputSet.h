#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <iostream>

#include "ConsoleColor.h"
#include "MyPaths .h"

class InputSet {
public:
    InputSet();
    void input();
    void save(std::string fname = "New input set");
private:
    std::vector<float> TimeDeltas;
    std::vector<std::string> Bitsets;
    std::string gameName, filename;
    int curCycle;
};
InputSet::InputSet() {
    curCycle = 0;
    Bitsets.clear();
    TimeDeltas.clear();
}

void InputSet::input() {
    bool goon = true;
    char line[11];// = { 0,0,0,0,0,0 };
    std::cin.getline(line, 10);// empty read because was pressed ENTER
    while (goon) {
        std::cout << "set # " << Bitsets.size() + 1 << "\t\t";
        std::cin.getline(line, 20);
        std::cin.clear();
        Beep(1100, 100);
        if (line[0] != '+') {
            Bitsets.push_back(line);
        }
        else goon = false;
    }
    for (int i = 0; i < Bitsets.size(); i++)
    {
        std::cout << Bitsets.at(i) << "\n";
    }
    std::cout << "Enter game name: ";
    std::cin.getline(line, 20);
    save(std::string(line));
}
void InputSet::save(std::string fname) {
    std::string path = MyPaths::getExecutableDir() + "\\Sets\\" + fname + ".txt";
    std::ofstream ofs(path, std::ios::trunc);
    for (int i = 0; i < Bitsets.size(); i++)
    {
        ofs << Bitsets.at(i) << "\n";
    }
    ofs.close();
}