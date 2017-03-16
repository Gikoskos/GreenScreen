#include "GRS.h"

HHOOK LLKeyboardHook = NULL;
HANDLE hKeyHookThread = NULL;
CRITICAL_SECTION csReadKeystroke;
KEYSTROKE g_keystroke; //key_is_pressed is initialized to 0 by default

static UINT CALLBACK LLKeyboardHookThread(PVOID args);
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM key_state, LPARAM keyhook_data);
static BOOL IsPrintableCharacter(DWORD vkCode);


UINT CALLBACK LLKeyboardHookThread(PVOID args)
{
    UNREFERENCED_PARAMETER(args);
    MSG msg;
    BOOL bRet;

    LLKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!LLKeyboardHook)
        exit(EXIT_FAILURE);

    while ((bRet = GetMessage(&msg, NULL, 0, 0))
           && (WaitForSingleObject(hKillThreadEvent, 0) != WAIT_OBJECT_0)) {

        if (bRet == -1)
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    _endthreadex(0);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM key_state, LPARAM keyhook_data)
{
    if (nCode >= 0) {

        EnterCriticalSection(&csReadKeystroke);
        g_keystroke.key_is_pressed = (key_state == WM_KEYDOWN);
        g_keystroke.virtual_code = ((PKBDLLHOOKSTRUCT)keyhook_data)->vkCode;
        g_keystroke.is_printable = IsPrintableCharacter(g_keystroke.virtual_code);
        g_keystroke.pressed_char = (WCHAR)MapVirtualKeyW(g_keystroke.virtual_code, MAPVK_VK_TO_CHAR);
        g_keystroke.alt_is_pressed = ((PKBDLLHOOKSTRUCT)keyhook_data)->flags & LLKHF_ALTDOWN;
        LeaveCriticalSection(&csReadKeystroke);

    }

    return CallNextHookEx(NULL, nCode, key_state, keyhook_data);
}

BOOL IsPrintableCharacter(DWORD vkCode)
{
    return (
            (
             (vkCode >= 0x30) //0 key
             &&
             (vkCode <= 0x39) //9 key
            )
            ||
            (
             (vkCode >= 0x41) //A key
             &&
             (vkCode <= 0x5A) //Z key
            )
            ||
            (vkCode == VK_SPACE)
            ||
            (vkCode == VK_BACK)
            ||
            (vkCode == VK_RETURN)
            ||
            (vkCode == VK_ESCAPE) //ESCAPE key isn't printable, I'm using it to exit the app for debugging
           );
}

void InitLLKeyboardHookThread(void)
{
    InitializeCriticalSection(&csReadKeystroke);

    hKeyHookThread = (HANDLE)_beginthreadex(NULL, 0, LLKeyboardHookThread, (PVOID)0, 0, NULL);
    if (!hKeyHookThread)
        exit(EXIT_FAILURE);
}
