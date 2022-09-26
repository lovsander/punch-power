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
//********************************************************************************************
// CLASS PATTERNED ENDED GAME PLAY
//********************************************************************************************

class EndedGamePlay : public Gameplay {
public:
    EndedGamePlay(int patternSize = 1, int numCycles = 10, std::string gameName = "ended chain");
    std::string gamename() { return gameName; }
    void begin();
    int run();
private:
    std::string gameName;
    int numCycles;
    int curCycle;
    int patternSize;

};

EndedGamePlay::EndedGamePlay(int patternSize, int numCycles, std::string gameName) {
    this->patternSize = patternSize;
    this->numCycles = numCycles;
    this->gameName = gameName;
}
void EndedGamePlay::begin() {
    curCycle = 0;
    Gameplay::begin();
}

int EndedGamePlay::run()
{
    if (curCycle < numCycles) {
        curCycle++;
        std::cout << white << curCycle << " of " << numCycles << ".  ";
        return Gameplay::play(patternSize);
    }
    else {
        ShowResults();
        return 2001;
    }
}

