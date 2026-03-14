#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <urlmon.h>
#include <thread>
#include <chrono>


#pragma comment(lib, "urlmon.lib")

DWORD GetProcessByName(const char* lpProcessName)
{
    char lpCurrentProcessName[255];

    PROCESSENTRY32 ProcList{};
    ProcList.dwSize = sizeof(ProcList);

    const HANDLE hProcList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcList == INVALID_HANDLE_VALUE)
        return -1;

    if (!Process32First(hProcList, &ProcList))
        return -1;

    wcstombs_s(nullptr, lpCurrentProcessName, ProcList.szExeFile, 255);

    if (lstrcmpA(lpCurrentProcessName, lpProcessName) == 0)
        return ProcList.th32ProcessID;

    while (Process32Next(hProcList, &ProcList))
    {
        wcstombs_s(nullptr, lpCurrentProcessName, ProcList.szExeFile, 255);

        if (lstrcmpA(lpCurrentProcessName, lpProcessName) == 0)
            return ProcList.th32ProcessID;
    }

    return -1;
}

int main()
{
    char appDataPath[MAX_PATH];
    const DWORD dwAppDataLen = GetEnvironmentVariableA("APPDATA", appDataPath, MAX_PATH);
    if (dwAppDataLen == 0 || dwAppDataLen > MAX_PATH)
    {
        printf("An error occured when trying to get APPDATA path.\n");
        return -1;
    }

    char misakiDir[MAX_PATH];
    char gmodDir[MAX_PATH];
    char dllPath[MAX_PATH];

    sprintf_s(misakiDir, "%s\\misaki", appDataPath);
    sprintf_s(gmodDir, "%s\\misaki\\gmod", appDataPath);
    sprintf_s(dllPath, "%s\\eb.dll", gmodDir);

    const DWORD dwGmodDirAttr = GetFileAttributesA(gmodDir);
    const DWORD dwDllAttr = GetFileAttributesA(dllPath);

    if (dwGmodDirAttr != INVALID_FILE_ATTRIBUTES &&
        (dwGmodDirAttr & FILE_ATTRIBUTE_DIRECTORY) &&
        dwDllAttr != INVALID_FILE_ATTRIBUTES &&
        !(dwDllAttr & FILE_ATTRIBUTE_DIRECTORY))
    {
        DeleteFileA(dllPath);
        RemoveDirectoryA(gmodDir);
        RemoveDirectoryA(misakiDir);

   //     printf("Previous eb.dll installation removed.\n");
    }

    CreateDirectoryA(misakiDir, nullptr);
    CreateDirectoryA(gmodDir, nullptr);

    const char* lpDownloadUrl = "dll link here";

    const HRESULT hrDownload = URLDownloadToFileA(nullptr, lpDownloadUrl, dllPath, 0, nullptr);
    if (FAILED(hrDownload))
    {
        printf("An error occured when trying to download the dll. HRESULT=0x%08lX\n", hrDownload);
        return -1;
    }

   // printf("eb.dll downloaded to %s\n", dllPath);

    const char* lpProcessName = "gmod.exe";

    DWORD dwProcessID = -1;

    printf("Waiting for %s to start...\n", lpProcessName);

    while (dwProcessID == (DWORD)-1)
    {
        dwProcessID = GetProcessByName(lpProcessName);

        if (dwProcessID == (DWORD)-1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    printf("%s detected!\n", lpProcessName);
    printf("[DLL Injector]\n");
    printf("Process : %s\n", lpProcessName);
    printf("Process ID : %i\n\n", (int)dwProcessID);
    printf("DLL path : %s\n\n", dllPath);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    

    const HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
    if (hTargetProcess == INVALID_HANDLE_VALUE)
    {
        printf("An error is occured when trying to open the target process.\n");
        return -1;
    }

    printf("[INJECTION]\n");
    printf("Process injected successfully.\n");

    const LPVOID lpPathAddress = VirtualAllocEx(hTargetProcess, nullptr, lstrlenA(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (lpPathAddress == nullptr)
    {
        printf("An error is occured when trying to allocate memory in the target process.\n");
        return -1;
    }

    printf("Memory allocate at 0x%X\n", (UINT)(uintptr_t)lpPathAddress);

    const DWORD dwWriteResult = WriteProcessMemory(hTargetProcess, lpPathAddress, dllPath, lstrlenA(dllPath) + 1, nullptr);
    if (dwWriteResult == 0)
    {
        printf("An error is occured when trying to write the DLL path in the target process.\n");
        return -1;
    }

    printf("DLL path writen successfully.\n");

    const HMODULE hModule = GetModuleHandleA("kernel32.dll");
    if (hModule == INVALID_HANDLE_VALUE || hModule == nullptr)
        return -1;

    const FARPROC lpFunctionAddress = GetProcAddress(hModule, "LoadLibraryA");
    if (lpFunctionAddress == nullptr)
    {
        printf("An error is occured when trying to get \"LoadLibraryA\" address.\n");
        return -1;
    }

    printf("LoadLibraryA address at 0x%X\n", (UINT)(uintptr_t)lpFunctionAddress);

    const HANDLE hThreadCreationResult = CreateRemoteThread(hTargetProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)lpFunctionAddress, lpPathAddress, 0, nullptr);
    if (hThreadCreationResult == INVALID_HANDLE_VALUE)
    {
        printf("An error is occured when trying to create the thread in the target process.\n");
        return -1;
    }

    printf("Injected!\n");

    return 0;
}