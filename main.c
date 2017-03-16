#include "GRS.h"

HINSTANCE g_hInstance = NULL;

static void CleanUp(void);

void CleanUp(void)
{
    if (screenBuff) DeleteBackbuffer(screenBuff);
    if (fntConsole) DeleteObject((HGDIOBJ)fntConsole);

    if (User.szName) win_free(User.szName);

    if (LLKeyboardHook) UnhookWindowsHookEx(LLKeyboardHook);

    if (hKillThreadEvent) {
        HANDLE hThreads[] = {
            hDataRprThread, hPCStatusThread
        };

        SetEvent(hKillThreadEvent);

        WaitForMultipleObjects(ARRAYSIZE(hThreads), hThreads, TRUE, INFINITE);

        for (int i = 0; i < ARRAYSIZE(hThreads); i++) {
            if (hThreads[i])
                CloseHandle(hThreads[i]);
        }

        CloseHandle(hKillThreadEvent);
    }

    DeleteCriticalSection(&csReadPCData);
    DeleteCriticalSection(&csReadKeystroke);
    DeleteCriticalSection(&csReadUserData);
}

int APIENTRY WinMain(
                     HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     PSTR pCmdLine,
                     int nCmdShow
                    )
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    g_hInstance = hInstance;

    _onexit((_onexit_t)CleanUp);

    return Run();
}
