// Description: The code checks for Bloxstrap or Roblox installations. 
//              It then retrieves the file path using registry keys and a JSON file.
//              Finally, it launches the program as eurotrucks2.exe to disguise it as an Nvidia Ansel compatible game.
// Author: Dante (dante@extravi.dev)
// Date: 2024-06-14

#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <codecvt>
#include <cstring>
#include <Windows.h>
#include <tlhelp32.h>
#include <shellapi.h>
#include "Discord.h"
#include <time.h>
#include <future>
#include <chrono>
static int64_t eptime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

#include "json.hpp"

using json = nlohmann::json;

// variables 
std::string arguments;
wchar_t value[MAX_PATH];
DWORD valueSize = sizeof(value);
bool bloxstrap = false;
std::string bloxstrapPath;
bool skip = false;

// turn off warning for 4996
#pragma warning(disable : 4996)

bool IsProcessRunning(int pid) {
    bool isRunning = false;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe)) {
            do {
                if (pe.th32ProcessID == pid) {
                    isRunning = true;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe));
        }
        CloseHandle(hSnapshot);
    }
    return isRunning;
}

void SleepFor(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

Discord* g_Discord;

void Discord::Initialize()
{
    DiscordEventHandlers Handle;
    memset(&Handle, 0, sizeof(Handle));
    Discord_Initialize("1247657703266975775", &Handle, 1, NULL);
}

void Discord::Update()
{
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.state = "Playing Roblox";
    discordPresence.startTimestamp = eptime;
    discordPresence.largeImageKey = "https://extravi.dev/update/RobloxRPC2.png";
    discordPresence.largeImageText = "Playing Roblox with Bloxshade";
    discordPresence.smallImageKey = "https://extravi.dev/update/RobloxRPCAsset.png";
    discordPresence.smallImageText = "Playing Roblox";
    Discord_UpdatePresence(&discordPresence);
}

std::wstring convertToWideString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

std::wstring convertToWidePath(const std::string& path) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), (int)path.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, path.c_str(), (int)path.size(), &wstr[0], size_needed);
    return wstr;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    std::locale::global(std::locale("en_US.UTF-8"));
    std::cout.imbue(std::locale());

    LPWSTR* argv;
    int argc;
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    // process arguments
    std::wstring wide_arguments;
    for (int i = 1; i < argc; ++i) {
        std::wstring warg(argv[i]);
        wide_arguments += L"\"" + warg + L"\" ";
    }

    // free memory allocated by CommandLineToArgvW
    LocalFree(argv);

    // roblox path
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Classes\\roblox-player\\shell\\open\\command", nullptr, RRF_RT_REG_SZ, nullptr, value, &valueSize);

    // convert string
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::wstring wstrValue(value);
    std::string robloxPath = converter.to_bytes(wstrValue);

    // check if path is empty
    if (robloxPath == "") {
        // show message box
        int result = MessageBoxW(nullptr, L"Roblox installation was not found. Do you want to download it?", L"Warning", MB_OKCANCEL | MB_ICONWARNING);
        if (result == IDOK) {
            ShellExecuteW(nullptr, L"open", L"https://www.roblox.com/download/client", nullptr, nullptr, SW_SHOWNORMAL);
        }
        return 0;
    }

    // print the reg key
    std::cout << robloxPath << std::endl;

    // extract the path
    size_t start = robloxPath.find('"') + 1;
    size_t end = robloxPath.rfind('"');
    std::string path = robloxPath.substr(start, end - start);

    // check the path
    std::cout << "Path to check: " << path << std::endl; // show the path
    if (path.find("C:\\Program Files") == 0 || path.find("C:\\Program Files (x86)") == 0) {
        std::cout << "Program files is true" << std::endl;
        MessageBox(NULL, L"It seems like Roblox is installed system-wide in the Program Files directory. Please install Roblox in a location other than the Program Files directory.", L"Information", MB_OK | MB_ICONWARNING);
        return 0;
    }
    else {
        std::cout << "Program files is false" << std::endl;
    }

    // check if path is under the users directory
    if (path.find("C:\\Users") == 0) {
        std::cout << "Users path is true" << std::endl;
    }
    else {
        std::cout << "Users path is false" << std::endl;
        std::cout << path << std::endl;
    }

    // Roblox seems to be installled from reg key
    std::cout << "Roblox install found" << std::endl;

    // bloxstrap
    size_t BloxstrapPos = path.find("Bloxstrap.exe");

    // roblox
    size_t RobloxPos = path.find("RobloxPlayerBeta.exe");

    if (BloxstrapPos != std::string::npos) {
        path.replace(BloxstrapPos, strlen("Bloxstrap.exe"), "Roblox\\Player");
        bloxstrap = true;
    }

    if (RobloxPos != std::string::npos) {
        path.replace(RobloxPos, strlen("RobloxPlayerBeta.exe"), "eurotrucks2.exe");
    }

    if (bloxstrap) {
        std::cout << "Bloxstrap is true" << std::endl;
        size_t pos = path.find("Roblox\\Player");
        if (pos != std::string::npos) {
            path = path.substr(0, pos + strlen("Roblox\\Player"));
        }

        bloxstrapPath = path + "\\eurotrucks2.exe";
        //std::cout << bloxstrapPath << std::endl;
    }
    else {
        std::cout << "Bloxstrap is false" << std::endl;
    }

    // set discord rpc
    if (!bloxstrap) {
        g_Discord->Initialize();
        g_Discord->Update();
    }

    // combine the path and arguments
    std::wstring wPath = convertToWidePath(bloxstrap ? bloxstrapPath : path);
    std::wstring completeArguments = L"\"" + wPath + L"\" " + wide_arguments;
    LPWSTR writable_arguments_ptr = (LPWSTR)completeArguments.c_str();

    PROCESS_INFORMATION ProcessInfo;
    STARTUPINFO StartupInfo;
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);

    int pid = 0;
    if (CreateProcessW(NULL, writable_arguments_ptr, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo)) {
        pid = ProcessInfo.dwProcessId; // process id
        //std::cout << pid << std::endl;

        CloseHandle(ProcessInfo.hThread);
        CloseHandle(ProcessInfo.hProcess);
    }

    if (pid == 0) {
        std::cout << "process does not exist" << std::endl;
        return 0; // exit process did not return or pid or the file does not exist
    }

    while (true) {
        if (IsProcessRunning(pid)) {
            // process exist
        }
        else {
            // process does not exist
            break;
        }
        std::future<void> future = std::async(std::launch::async, []() {
            SleepFor(1000);
            });
        future.wait();
    }

    // debug to read output won't matter anyway since this will be a hidden window
    //std::cin.get();
    return 0;
}
