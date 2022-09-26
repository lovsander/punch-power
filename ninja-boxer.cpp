// ninja-boxer.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <tuple>

#include "ConsoleColor.h"
#include "Gameplay.h"
#include "InputSet.h"



// class InterfaceGamePlay
//*****************************************************************************************************************************
class BaseInterface {
public:
    void begin();
    int run();
    inline void setNumCycles(int numCyc) { numCycles = numCyc; }
    inline void setPatternSize(int pattSize) { patternSize = pattSize; }
protected:
    BaseInterface();
    ~BaseInterface();

    void setAims(std::string aims) { this->aims = aims; }
    void setDelay(float delay) { this->delay = delay; }
    float randDelay(float max);
    inline int getCurCycle() {return curCycle;}
    inline float getDelay() { return delay; }
    inline std::string getAims() { return aims; }
    inline int getPatternSize() { return patternSize; }
    virtual void updateAims() { std::cout << "Base::update\n"; }

private:
    Gameplay *gamePlay;
    std::string aims;
    int numCycles;
    int curCycle;
    int patternSize=3;
    float delay = 1.0;
    std::string gameName;   
};

BaseInterface::BaseInterface() {
    gamePlay = new Gameplay();
}

BaseInterface::~BaseInterface() {
   delete gamePlay;
}

void BaseInterface::begin() {
    curCycle = 0;
    gamePlay->begin();
}
float BaseInterface::randDelay(float max)
{
    int rnd = rand();
    float frand = (float)rnd / RAND_MAX;
    return frand * max;
}
int BaseInterface::run()
{
    if (curCycle < numCycles) {
        updateAims(); // virtual. see in derived classes
        curCycle++;
        std::cout << white << curCycle << " of " << numCycles << ".  ";
        return gamePlay->play(getAims(), getPatternSize(), getDelay());
    }
    else {
        gamePlay->ShowResults();
        return 2001;
    }
}


//*****************************************************************************************************************************
class RandGame : public BaseInterface {
public:
    void updateAims(); //virtual
    inline void setRandPatt(bool arg) { randPatt = arg; }

private:
    int getRnd(int max = 6);
    bool randPatt = false;
};

int RandGame::getRnd(int max) {
    int rnd = rand();
    float frand = (float)rnd / RAND_MAX;
    int ans = round(frand * (max - 1) + 1);
    return ans;
}

void RandGame::updateAims() {
    std::string aims;
    int pattSize = getPatternSize();
    setDelay(randDelay(2));
    // random fill
    if (randPatt) pattSize = getRnd(pattSize); // random pat mode
    aims.resize(pattSize);
    // fill pattern
    for (int i = 0; i < pattSize; i++)
    {
        _itoa(getRnd(), &aims[i],10);
    }
    setAims(aims);
}



//*****************************************************************************************************************************
class FileGame : public BaseInterface {
public:
    void begin();
    void updateAims(); //virtual
    inline void setFileName(std::string filename){ this->filename = filename; }
    inline void setGameName(std::string gamename) { this->gamename = gamename; }
protected:
    bool FileReadCsv();
    bool FileReadTxt();
private:
    std::vector<float> TimeDelays;
    std::vector<std::string> AimSets;
    std::string filename,gamename;
};
 
void FileGame::begin() {
    FileReadTxt();
    BaseInterface::setNumCycles(AimSets.size());
    BaseInterface::begin();
}

void FileGame::updateAims() {
    // sequence from a file
    setDelay(randDelay(2));     //need for txt file; but csv file must have self own delay values
    int curCycle = getCurCycle();
    std::string aimSet = AimSets.at(curCycle);
    setAims(aimSet);
    setPatternSize(aimSet.size());
}


bool FileGame::FileReadTxt()
{
    // fills  Bitsets only
    std::ifstream ifs;
    ifs.open(filename);
    if (ifs.is_open()) {
        std::cout << "txt file opened " << gamename << std::endl;
        std::string aimset;

        while (getline(ifs, aimset))
        {
            AimSets.push_back(aimset);
        }
        ifs.close();

        if (AimSets.size() == 0) {
            std::cout << red << " file is empty " << gamename << std::endl;
            return false;
        }
        std::cout << "patt" << "\n";
        for (int i = 0; i < AimSets.size(); i++)
        {
            std::cout << AimSets.at(i) << std::endl;
        }
        return true;
    }
    else {
        std::cout << red << "can't open file " << gamename << std::endl;
        return false;
    }
}

bool FileGame::FileReadCsv() {
    // fills Timelines & Bitsets
    std::ifstream ifs;
    ifs.open(filename);
    if (ifs.is_open()) {
        std::cout << "csv file opened" << std::endl;
        float timeDelay;

        std::vector<std::vector<std::string>> content;
        std::vector<std::string> row;
        std::string line, word, aimset;

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
            timeDelay = std::stof(content[i][0]);
            aimset = content[i][1];
            TimeDelays.push_back(timeDelay);
            AimSets.push_back(aimset);
            //for (int j = 0;j < content[i].size();j++)
            //    std::cout << content[i][j] << "\t ";
            //std::cout << "\n";
        }
        if (TimeDelays.size() != AimSets.size()) {
            std::cout << red << " file is corrupted" << std::endl;
            return false;
        }
        std::cout << "time" << "\t " << "patt" << "\n";
        for (int i = 0; i < TimeDelays.size(); i++)
        {
            std::cout << TimeDelays.at(i) << "\t " << AimSets.at(i) << std::endl;
        }
        return true;
    }
    else {
        std::cout << red << "can't open file " << std::endl;
        return false;
    }
}


bool listFiles(std::string *fn, std::string *gn) {
    // play selected stored file game
    std::cout << blue << " selected game is " << " : " << "stored file game" << std::endl;
    std::vector<std::string> fnames, fpath;
    int useGame = -1;
    while (useGame < 0) {
        namespace fs = std::filesystem;
        std::string path = MyPaths::getExecutableDir() + "\\Sets";
        int fnum = 1;
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".txt") {
                fpath.push_back(entry.path().string());
                std::string fn = entry.path().filename().replace_extension().string();
                std::cout << fnum << ". " << fn << std::endl;
                fnames.push_back(fn);
                fnum++;
            }
        }
        char selectedGame;
        std::cin >> selectedGame;
        if (isdigit(selectedGame)) {
            int n = selectedGame - 48 - 1;
            if (n >= 0 && n < fnames.size()) {
                useGame = n;
                *fn = fpath.at(useGame);
                *gn = fnames.at(useGame);
                return true;
            }
        }
        else {
            return false;
        }
    }
}

void fileGameSelect() {
    std::string fname;
    std::string gname;
    if (listFiles(&fname, &gname)) {
        FileGame* fg = new FileGame;
        fg->setFileName(fname);
        fg->setGameName(gname);
        fg->begin();
        while (fg->run() < 2000) {}
        delete fg;
        std::cout << blue << "COMPLETED!!!  ";
        Beep(1100, 500);
        system("pause");
    }
}

void randGameSelect(int nc, int ps) {
    RandGame* rg = new RandGame;
    rg->begin();
    rg->setNumCycles(nc);
    if (ps < 0) {// признак рандома - отрицат значение
        rg->setRandPatt(true);
        rg->setPatternSize(-1*ps);
    }
    else {
        rg->setRandPatt(false);
        rg->setPatternSize(ps);
    }

    while (rg->run() < 2000) {}
    delete rg;
    std::cout << blue << "COMPLETED!!!  ";
    Beep(1100, 500);
    system("pause");
}


void Menu() {
    // массив с рандомными вариантами
    std::vector<std::tuple<int, int, const char*> > RandGames;
    RandGames.push_back(std::make_tuple(2, 10, "duplets chain len 10"));
    RandGames.push_back(std::make_tuple(3, 20, "triplets chain len 20"));
    RandGames.push_back(std::make_tuple(-4, 15, "random(4) pat mode len 15"));
    RandGames.push_back(std::make_tuple(-3, 8, "random(3) pat mode len 8"));
    RandGames.push_back(std::make_tuple(-6, 21, "random(6) pat mode len 21"));

    Beep(1100, 300);

    while (1) { // main endless loop
        // select game
        std::cout << std::endl << white << "menu->select game mode: " << std::endl;
        int gnum = 0;
        for (const auto& g : RandGames)
        {
            gnum++;
            std::cout << yellow << gnum << ". " << std::get<2>(g) << std::endl;
        }
        gnum++;
        std::cout << std::endl << yellow << gnum << ". " << "stored File" << std::endl;
        gnum++;
        std::cout << std::endl << yellow << gnum << ". " << "INPUT SET" << std::endl;


        // treat input
        char selectedGame;
        std::cin >> selectedGame;
        if (isdigit(selectedGame)) {
            int n = selectedGame - 48 - 1;//need zero-starting index instead of 1-starting
            if (n >= 0 && n < RandGames.size()) {
                int useGame = n;
                // play selected random game
                std::cout << blue << " selected game is " << " : " << std::get<2>(RandGames.at(useGame)) << std::endl;
                randGameSelect(std::get<1>(RandGames.at(useGame)), std::get<0>(RandGames.at(useGame)));

            }
            else if (n == RandGames.size()) {
                // play selected stored file game
                std::cout << blue << " selected game is " << " : " << "stored file game" << std::endl;
                std::vector<std::string> fnames, fpath;
                int useGame = -1;
                while (useGame < 0) {
                    namespace fs = std::filesystem;
                    std::string path = MyPaths::getExecutableDir() + "\\Sets";
                    int fnum = 1;
                    for (const auto& entry : fs::directory_iterator(path)) {
                        if (entry.path().extension() == ".txt") {
                            fpath.push_back(entry.path().string());
                            std::string fn = entry.path().filename().replace_extension().string();
                            std::cout << fnum << ". " << fn << std::endl;
                            fnames.push_back(fn);
                            fnum++;
                        }
                    }
                    char selectedGame;
                    std::cin >> selectedGame;
                    if (isdigit(selectedGame)) {
                        int n = selectedGame - 48 - 1;
                        if (n >= 0 && n < fnames.size()) {
                            useGame = n;
                        }
                    }
                    else {

                    }
                }
                fileGameSelect();
            }
            else if (n == RandGames.size()+1) { // traner input
                std::cout << blue << " selected INPUT SET " << std::endl;
                std::cout << blue << " input digits as sets , non-digit for completion" << std::endl;
                //system("pause");

                InputSet* inpset = new InputSet();
                inpset->input();
                delete inpset;
            }
        }
        else break;
        //}

    }
}


bool killFiles() {
    std::string* fn;
    // play selected stored file game
    std::cout << blue << " kill many files " << std::endl;
    std::vector<std::string> fnames, fpath;
        namespace fs = std::filesystem;
        std::string path = "\\192.168.51.133\\Disk_C$\\REG3XML\\done01";
        int fnum = 1;
        for (const auto& entry : fs::directory_iterator(path)) {
        std:remove(entry.path());
            //fpath.push_back(entry.path().string());
            std::cout << blue << fnum<< " "<< entry.path() << std::endl;
            fnum++;
        }
}


int main()
{
    killFiles();
    return 0;
   Menu();
   std::cout << blue << "Exit " << std::endl;
   system("pause");
   return 0;
}
