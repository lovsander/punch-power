#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <time.h>
#include "ConsoleColor.h"

//********************************************************************************************
// BASE CLASS ENDLESS GAME PLAY
//********************************************************************************************

class Gameplay {
    //endless chain
public:
    void begin();
    //int play(int pattSize = 1);
    int play(std::string aims, int pattSize = 1, float pause=2);
    void setOptions() { ; }
    void ShowResults();
    int getNumErrs() { return numErrs; }
    int getGeneralCounter() { return generalCounter; }
    float getSumReactTime() { return sumReactTime; }
    float getGameTime() { return gameTime; }
private:
    void delay(float pause=2);
    void showAims(std::string aims, int pattSize);
   // int* getAims(int* pattSize);
    int* getGots(int pattSize, bool* exit);
    int checkCorrect(int pattSize, std::string aims, int* gots);
    float randDelay(float max = 3.0);
    void startGame() { stgm = clock(); }
    void startTimer() { sttm = clock(); }
    bool isZeroReact();
    void endTimer();
    void updateGameTime();
    int getRnd(int max = 6);
    //work vars

    char ans;
    long long sttm, entm;
    int reaction;
    float secReaction;
    //statistics vars
    int numErrs;
    int generalCounter;
    float sumReactTime;
    //game time vars
    long long stgm, engm;
    float gameTime;
};

void Gameplay::begin() {
    srand(clock());
    startGame();
    numErrs = 0;
    generalCounter = 0;
    sumReactTime = 0;
}
int Gameplay::getRnd(int max) {
    int rnd = rand();
    float frand = (float)rnd / RAND_MAX;
    return round(frand * (max - 1) + 1);
}
float Gameplay::randDelay(float max)
{
    int rnd = rand();
    float frand = (float)rnd / RAND_MAX;
    return frand * max;
}
bool Gameplay::isZeroReact() {
    if (clock() - sttm == 0) return true; // in case when more than one symbol got - time diff = 0
    else return false;
}
void Gameplay::endTimer() {
    entm = clock();
    reaction = entm - sttm;
    secReaction = (float)reaction / 1000;
    sumReactTime += secReaction;
    generalCounter += 1;
}
void Gameplay::updateGameTime() {
    engm = clock();
    gameTime = (float)(engm - stgm) / 1000;
}
/*
int* Gameplay::getAims(int* pattSize) {
    int* aims;
    if (*pattSize > 1000) {// sequence from a file
        int sequence = *pattSize - 1000;
        std::string str = std::to_string(sequence);
        *pattSize = str.size();
        aims = new int[*pattSize];
        // fill pattern
        std::cout << yellow << "press ";// << pattSize << " nums ... ";
        for (int i = 0; i < *pattSize; i++)
        {
            *(aims + i) = str.at(i) - 48;
            std::cout << *(aims + i);
        }
        std::cout << " ... ";
    }
    else { // random fill
        if (*pattSize > 8) *pattSize = 8;  // restriction by BYTE bitAns
        if (*pattSize < 0) *pattSize = getRnd(-1 * *pattSize); // random pat mode
        aims = new int[*pattSize];
        // fill pattern
        std::cout << yellow << "press ";// << pattSize << " nums ... ";
        for (int i = 0; i < *pattSize; i++)
        {
            *(aims + i) = getRnd();
            std::cout << *(aims + i);
        }
        std::cout << " ... ";
    }
    return aims;
}
*/
int* Gameplay::getGots(int pattSize, bool* exit) {
    int got = 0;
    int* gots = new int[pattSize];
    while (got < pattSize) {
        char ans;
        std::cin >> ans;
        if (isZeroReact()) continue;
        // check zero-rection! filtrate overCounts
        if (isdigit(ans)) {
            gots[got] = ans - 48;
            got++;
        }
        else {
            std::cout << blue << "get " << ans << " EXIT " << std::endl;
            *exit = true;
            break;
        }
    }
    return gots;
}

int Gameplay::checkCorrect(int pattSize, std::string aims, int* gots) {
    unsigned char bitAns = 0;
    bool allright = true;
    for (int i = 0; i < pattSize; i++)
    {
        if (aims[i]-48 == gots[i]) {
            bitAns += 2 ^ i;// assemble bit by correct aim gots
            std::cout << green << gots[i] << " ok, ";
        }
        else {
            std::cout << red << gots[i] << " err ";
            allright = false;
        }
    }
    std::cout << secReaction << " sec " << std::endl;
    if (allright)    return 1000 + bitAns; // to distinct by other answers
    else {
        numErrs++;
        return -1 * (1000 + bitAns); // reverse answer if err
    }
}

void Gameplay::delay(float pause) {
   // float pause = randDelay(2);
    std::cout << std::fixed << std::setprecision(2) << "pause " << pause << "\t\t\t";
    Sleep(pause * 1000);
}
void Gameplay::showAims(std::string aims, int pattSize) {
    std::cout << yellow << "press ";// << pattSize << " nums ... ";
    for (int i = 0; i < pattSize; i++)
    {
        std::cout << aims[i];
    }
    std::cout << " ... ";
}

//new version
int Gameplay::play(std::string aims, int pattSize, float pause) {
    int* gots;
    bool exit = false;
    delay(pause);
    showAims(aims, pattSize);
    startTimer();
    //... here we wait for player react
    gots = getGots(pattSize, &exit);
    if (exit) return 2000;
    endTimer();
    updateGameTime();
    return checkCorrect(pattSize, aims, gots);
}
// old version
/*int Gameplay::play(int pattSize) {
    int* aims, * gots;
    bool exit = false;

    delay();
    aims = getAims(&pattSize);
    startTimer();
    //... here we wait for player react
    gots = getGots(pattSize, &exit);
    if (exit) return 2000;
    endTimer();
    updateGameTime();
    return checkCorrect(pattSize, aims, gots);
}*/

void Gameplay::ShowResults() {
    std::cout << std::endl << blue << "-------- YOUR RESULTS -------" << std::endl;
    std::cout << "GeneralCounter " << getGeneralCounter() << std::endl;
    std::cout << "NumErrs " << getNumErrs() << std::endl;
    std::cout << "TotalGameTime " << std::fixed << std::setprecision(1) << getGameTime() << std::endl;
    std::cout << "SumReactTime  " << std::fixed << std::setprecision(1) << getSumReactTime() << std::endl;
    std::cout << "Percentage done " << std::fixed << std::setprecision(0) << 100 * (float)(getGeneralCounter() - getNumErrs()) / getGeneralCounter() << " %" << std::endl;
    std::cout << "AvgReactTime " << std::fixed << std::setprecision(1) << getSumReactTime() / getGeneralCounter() << std::endl;
    std::cout << blue << "-------------------------" << std::endl;
}