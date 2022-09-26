#ifndef MyPathsH
#define MyPathsH
//#pragma once

#include <string>
namespace MyPaths {
    std::string getExecutablePath();
    std::string getExecutableDir();
}


#include <windows.h>
#include <Shlwapi.h>
#include <io.h> 
#include <pathcch.h>



namespace MyPaths {


    std::string getExecutablePath() {
        char rawPathName[MAX_PATH];
        GetModuleFileNameA(NULL, rawPathName, MAX_PATH);
        return std::string(rawPathName);
    }

    std::string getExecutableDir() {

        std::string executablePath = getExecutablePath();
        
       // _splitpath()
        //strcpy_s(exePath, executablePath.length(), executablePath.c_str());
        //PathRemoveFileSpecA(exePath);
        //PathCchRemoveFileSpec(exePath, executablePath.length());
       // std::string directory = std::string(exePath);
        //delete[] exePath;
        return executablePath.substr(0, executablePath.find_last_of("\\/"));
    }


}

#endif // !MyPathsH