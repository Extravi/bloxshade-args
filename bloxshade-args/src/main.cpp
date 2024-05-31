// Description: The code checks for Bloxstrap or Roblox installations. 
//              It then retrieves the file path using registry keys and a JSON file.
//              Finally, it launches the program as eurotrucks2.exe to disguise it as an Nvidia Ansel compatible game.
// Author: Dante (dante@extravi.dev)
// Date: 2024-04-22

#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <codecvt>
#include <cstring>
#include <Windows.h>
#include <lmcons.h>
#include <shellapi.h>

#include "json.hpp"

using json = nlohmann::json;

// variables 
std::string arguments;
wchar_t value[MAX_PATH];
DWORD valueSize = sizeof(value);
bool bloxstrap = false;
std::string bloxstrapPath;

// turn off warning for 4996
#pragma warning(disable : 4996)

// 8.3 format
std::wstring GetShortUsername() {
    // current username
    wchar_t username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (!GetUserNameW(username, &username_len)) {
        // failed to get the username
        std::cout << "failed to get the username: " << GetLastError() << std::endl;
        return L"";
    }

    // Get the user profile path using environment variable
    wchar_t userProfilePath[MAX_PATH];
    if (!GetEnvironmentVariableW(L"USERPROFILE", userProfilePath, MAX_PATH)) {
        // failed to get the user profile path
        std::cout << "failed to get the user profile path: " << GetLastError() << std::endl;
        return L"";
    }

    // Get the short path form of the user's home directory
    DWORD bufferSize = GetShortPathNameW(userProfilePath, NULL, 0);
    if (bufferSize == 0) {
        // failed to get the short path name
        std::cout << "failed to get the short path name: " << GetLastError() << std::endl;
        return L"";
    }

    std::vector<wchar_t> shortPathBuffer(bufferSize);
    if (GetShortPathNameW(userProfilePath, shortPathBuffer.data(), bufferSize) == 0) {
        // failed to retrieve the short path name
        std::cout << "failed to retrieve the short path name: " << GetLastError() << std::endl;
        return L"";
    }

    std::wstring shortPath(shortPathBuffer.data());

    // Extract the short username
    size_t pos = shortPath.find_last_of(L"\\");
    if (pos == std::wstring::npos) {
        // failed to extract short username from path
        std::cout << "failed to extract short username from path: " << GetLastError() << std::endl;
        return L"";
    }

    return shortPath.substr(pos + 1);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    LPWSTR* argv;
    int argc;
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    // process arguments
    std::string arguments;
    for (int i = 1; i < argc; ++i) {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr, nullptr);
        std::string narrow_string(size_needed - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, &narrow_string[0], size_needed, nullptr, nullptr);
        arguments += "\"" + narrow_string + "\" ";
    }

    // free memory allocated by CommandLineToArgvW
    LocalFree(argv);

    // roblox path
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Classes\\roblox-player\\shell\\open\\command", nullptr, RRF_RT_REG_SZ, nullptr, value, &valueSize);

    // convert string
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::wstring wstrValue(value);
    std::string robloxPath = converter.to_bytes(wstrValue);

    // 8.3 username format
    std::wstring shortUsername = GetShortUsername();
    std::string newShortUsername = converter.to_bytes(shortUsername);
    if (!shortUsername.empty()) {
        std::cout << "Short username: " << newShortUsername << std::endl;
    }
    else {
        std::cout << "failed to get the short username" << std::endl;
    }

    // replace username from robloxPath
    wchar_t username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserNameW(username, &username_len)) {
        std::string narrowUsername = converter.to_bytes(username);
        size_t pos = robloxPath.find(narrowUsername);
        if (pos != std::string::npos) {
            robloxPath.erase(pos, narrowUsername.length());
            robloxPath.insert(pos, newShortUsername); // replace with 8.3 username format
        }
        else {
            std::cout << "Username not found in the path" << std::endl;
            std::vector<size_t> slashPositions;
            // check if it is less than 3
            if (slashPositions.size() < 3) {
                // extract the path
                size_t start = robloxPath.find('"') + 1;
                size_t end = robloxPath.rfind('"');
                std::string path = robloxPath.substr(start, end - start);
                std::string roPath = robloxPath.substr(start, end - start);

                // check the path
                std::cout << "Path to check: " << path << std::endl;
                if (path.find("C:\\Program Files") == 0 || path.find("C:\\Program Files (x86)") == 0) {
                    std::cout << "Program files is true" << std::endl;
                    MessageBox(NULL, L"It seems like Roblox is installed system-wide in the Program Files directory. Please install Roblox in a location other than the Program Files directory.", L"Information", MB_OK | MB_ICONWARNING);
                    return 0;
                }
                else {
                    std::cout << "Program files is false" << std::endl;
                }

                std::cout << "Roblox path does not seem to exist" << std::endl;
                // show message box
                int result = MessageBoxW(nullptr, L"Roblox installation was not found. Do you want to download it?", L"Warning", MB_OKCANCEL | MB_ICONWARNING);
                if (result == IDOK) {
                    ShellExecuteW(nullptr, L"open", L"https://www.roblox.com/download/client", nullptr, nullptr, SW_SHOWNORMAL);
                }
                return 0;
            }
            // find "/" pos
            for (size_t i = 0; i < robloxPath.length(); ++i) {
                if (robloxPath[i] == '\\') {
                    slashPositions.push_back(i);
                }
            }
            size_t secondSlashPos = slashPositions[1];
            size_t thirdSlashPos = slashPositions[2];
            std::string newRobloxPath = robloxPath.substr(0, secondSlashPos + 1) + newShortUsername + robloxPath.substr(thirdSlashPos);
            // set new path
            robloxPath = newRobloxPath;
            std::cout << "We set a new path trying something else" << std::endl;
        }
    }
    else {
        std::cout << "failed to get the username" << std::endl;
    }

    // Roblox seems to be installled from reg key
    std::cout << "Roblox install found" << std::endl;

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
        catch (json::parse_error&) {
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
