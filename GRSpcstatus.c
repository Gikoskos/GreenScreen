#include "GRS.h"

HANDLE hPCStatusThread;
CRITICAL_SECTION csReadPCData;
PC this_system = {
    .brInstalled = NO_BROWSER
};

static UINT CALLBACK PCStatusThread(PVOID args);
static WORD GetSystemArchitecture(void);
static WEB_BROWSER FirefoxMozillaInstallStatus(void);


UINT CALLBACK PCStatusThread(PVOID args)
{
    UNREFERENCED_PARAMETER(args);

    EnterCriticalSection(&csReadPCData);
    this_system.arch = GetSystemArchitecture();
    this_system.isWow64 = (this_system.arch == PROCESSOR_ARCHITECTURE_AMD64) ||
                          (this_system.arch == PROCESSOR_ARCHITECTURE_IA64);
    LeaveCriticalSection(&csReadPCData);

    EnterCriticalSection(&csReadPCData);
    this_system.brInstalled = FirefoxMozillaInstallStatus();
    LeaveCriticalSection(&csReadPCData);

    _endthreadex(0);
}

WORD GetSystemArchitecture(void)
{
    BOOL isWow64proc = FALSE;
    SYSTEM_INFO infSys;
    BOOL (__stdcall *fnIsWow64Process)(HANDLE, PBOOL) = NULL;
    HMODULE kernel32dll = LoadLibraryW(L"kernel32.dll");

    if (kernel32dll) {
        fnIsWow64Process = (BOOL(__stdcall *)(HANDLE, PBOOL))GetProcAddress(kernel32dll, "IsWow64Process");

        if (fnIsWow64Process) {
            if (!fnIsWow64Process(GetCurrentProcess(),&isWow64proc))
                exit(EXIT_FAILURE);
        }

        FreeLibrary(kernel32dll);
    }

    if (isWow64proc) {
        GetNativeSystemInfo(&infSys);
    } else {
        GetSystemInfo(&infSys);
    }

    return infSys.wProcessorArchitecture;
}

WEB_BROWSER FirefoxMozillaInstallStatus(void)
{
    //First method to check if firefox exists, is to see if it is installed in the predefined dirs
    PWSTR szPossibleFirefoxDirs[] = {
        L"C:\\Program Files\\Mozilla Firefox\\firefox.exe",
        L"C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe"
    };
    HANDLE hFirefox;

    for (int i = 0; i < ARRAYSIZE(szPossibleFirefoxDirs); i++) {
        hFirefox = CreateFileW(szPossibleFirefoxDirs[i], GENERIC_READ, FILE_SHARE_READ,
                               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFirefox != INVALID_HANDLE_VALUE) {
            CloseHandle(hFirefox);
            return FIREFOX_BROWSER;
        }
    }

    //Second method to check if firefox exists, is to look if the Firefox registry keys exist
    PWSTR szPossibleRegKeys[] = {
        L"Software\\Clients\\StartMenuInternet\\FIREFOX.EXE",
        L"Software\\Mozilla\\Firefox"
    };
    HKEY hkFirefox;
    BOOL isWow64;
    DWORD disp;
    LONG ret;

    for (int i = 0; i < ARRAYSIZE(szPossibleRegKeys); i++) {
        ret = RegCreateKeyExW(HKEY_CURRENT_USER, szPossibleRegKeys[i], 0,
                              NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL,
                              &hkFirefox, &disp);

        RegCloseKey(hkFirefox);
        if (ret == ERROR_SUCCESS) {
            //if even one key doesn't exist, then firefox isn't installed
            if (disp != REG_OPENED_EXISTING_KEY) {
                EnterCriticalSection(&csReadPCData);
                isWow64 = this_system.isWow64;
                LeaveCriticalSection(&csReadPCData);

                RegDeleteKeyEx(HKEY_CURRENT_USER,
                               szPossibleRegKeys[i],
                               (isWow64) ? KEY_WOW64_64KEY : KEY_WOW64_32KEY, 0);
                break;
            } else {
                //if all the keys exist then firefox is installed (? not 100% sure it works)
                if  (i == ARRAYSIZE(szPossibleRegKeys) - 1)
                    return FIREFOX_BROWSER;
            }
        }        
    }

    return NO_BROWSER;
}

void FFProfileDirs(void)
{
    /*size_t path_len;
    WIN32_FIND_DATA ffd;
    HANDLE hDir;
    WCHAR szTmp1[] = L"C:\\Users\\";
    WCHAR szTmp2[] = L"\\AppData\\Roaming\\Mozilla\\Firefox\\Profiles\\*";
    WCHAR *szPath;

    if (FAILED(StringCchLengthW(User.szName, UNLEN + 1, &path_len)))
        exit(EXIT_FAILURE);

    path_len = path_len + ARRAYSIZE(szTmp1) + ARRAYSIZE(szTmp2) - 1;
    if (!(szPath = HeapAlloc(GetProcessHeap(), 0, path_len * sizeof(WCHAR))))
        exit(EXIT_FAILURE);

    if (FAILED(StringCchPrintfW(szPath, path_len, L"%s%s%s", szTmp1, User.szName, szTmp2)))
        exit(EXIT_FAILURE);

    wprintf(L"%s\n", szPath);

    hDir = FindFirstFile(szPath, &ffd);
    if (hDir == INVALID_HANDLE_VALUE)
        exit(EXIT_FAILURE);

    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            size_t dir_len;

            if (FAILED(StringCchLengthW(ffd.cFileName, MAX_PATH, &dir_len)))
                exit(EXIT_FAILURE);

            if (!(szPath = HeapReAlloc(GetProcessHeap(), 0, szPath, (path_len + dir_len) * sizeof(WCHAR))))
                exit(EXIT_FAILURE);

            
        }
    } while (FindNextFile(hDir, &ffd) != 0);

    FindClose(hDir);
    HeapFree(GetProcessHeap(), 0, szPath);*/
}

void InitPCStatusThread(void)
{
    InitializeCriticalSection(&csReadPCData);

    hPCStatusThread = (HANDLE)_beginthreadex(NULL, 0, PCStatusThread, (PVOID)0, 0, NULL);
    if (!hPCStatusThread)
        exit(EXIT_FAILURE);
}
