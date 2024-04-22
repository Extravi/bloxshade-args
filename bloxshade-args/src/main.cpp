// Description: The code checks for Bloxstrap or Roblox installations. 
//              It then retrieves the file path using registry keys and a JSON file.
//              Finally, it launches the program as eurotrucks2.exe to disguise it as an Nvidia Ansel compatible game.
// Author: Dante (dante@extravi.dev)
// Date: 2024-04-22

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <Windows.h>
#include <shellapi.h>

#include "json.hpp"

using json = nlohmann::json;

// variables 
std::string arguments;
char value[MAX_PATH];
DWORD valueSize = sizeof(value);
bool bloxstrap = false;
std::string bloxstrapPath;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    LPWSTR* argv;
    int argc;
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    // process arguments
    std::string arguments;
    for (int i = 1; i < argc; ++i) {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr, nullptr);
        std::string narrow_string(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, &narrow_string[0], size_needed, nullptr, nullptr);
        arguments += "\"" + narrow_string + "\" ";
    }

    // free memory allocated by CommandLineToArgvW
    LocalFree(argv);

    // roblox path
    RegGetValueA(HKEY_CLASSES_ROOT, "roblox-player\\shell\\open\\command", nullptr, RRF_RT_REG_SZ, nullptr, value, &valueSize);

    // convert to C++ string
    std::string robloxPath(value);

    if (robloxPath == "") {
        // prints are for me when im testing this
        std::cout << "Roblox is not installed" << std::endl;
        // show message box
        int result = MessageBoxW(nullptr, L"Roblox installation was not found. Do you want to download it?", L"Warning", MB_OKCANCEL | MB_ICONWARNING);
        if (result == IDOK) {
            ShellExecuteW(nullptr, L"open", L"https://www.roblox.com/download/client", nullptr, nullptr, SW_SHOWNORMAL);
        }
        return 0;
    }
    else {
        std::cout << "Roblox install found" << std::endl;
    }

    // extract the path
    size_t start = robloxPath.find('"') + 1;
    size_t end = robloxPath.rfind('"');
    std::string path = robloxPath.substr(start, end - start);

    // bloxstrap
    size_t BloxstrapPos = path.find("Bloxstrap.exe");

    // roblox
    size_t RobloxPos = path.find("RobloxPlayerBeta.exe");

    if (BloxstrapPos != std::string::npos) {
        path.replace(BloxstrapPos, strlen("Bloxstrap.exe"), "State.json");
        bloxstrap = true;
    }

    if (RobloxPos != std::string::npos) {
        path.replace(RobloxPos, strlen("RobloxPlayerBeta.exe"), "eurotrucks2.exe");
    }

    if (bloxstrap) {
        std::cout << "Bloxstrap is true" << std::endl;

        // read json
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "failed to open file" << std::endl;
            return 1;
        }

        json data;
        try {
            file >> data;
        }
        catch (json::parse_error& e) {
            std::cerr << "failed to read file" << std::endl;
            return 1;
        }

        std::cout << data["VersionGuid"] << std::endl;

        // set new path
        std::string versionGuid = data["VersionGuid"];
        std::string versionsDir = "Versions\\";
        path.replace(BloxstrapPos, strlen("State.json"), versionsDir + versionGuid);

        bloxstrapPath = path + "\\eurotrucks2.exe";
    }
    else {
        std::cout << "Bloxstrap is false" << std::endl;
    }

    // combine the path and arguments
    if (bloxstrap) {
        std::string command = "\"" + bloxstrapPath + "\" " + arguments;

        // print command
        std::cout << command << std::endl;

        // start roblox
        // yes i know WinExec is deprecated but idc
        WinExec(command.c_str(), SW_HIDE);
    }
    else {
        std::string command = "\"" + path + "\" " + arguments;

        // print command
        std::cout << command << std::endl;

        // start roblox
        WinExec(command.c_str(), SW_HIDE);
    }

    // debug to read output won't matter anyway since this will be a hidden window
    //std::cin.get();
    return 0;
}