#include "GRS.h"


BACKBUFFER *screenBuff = NULL;
HANDLE hKillThreadEvent = NULL;
HWND hScreen = NULL;
HFONT fntConsole;
BOOL exitApp = FALSE;
DWORD fontSize = 30;


int Run(void)
{
    hKillThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!hKillThreadEvent)
        exit(EXIT_FAILURE);

    InitPCStatusThread();
    InitDataReaperThread();
    InitLLKeyboardHookThread();


    if ((hScreen = GetDesktopWindow())) {
        if ((screenBuff = NewBackbuffer(hScreen))) {

            //font scaling algorithm is based on the fact that
            //for a screen resolution of 1920x1080, we get 30px font size
            fontSize = (30 * screenBuff->rc.right) / 1920;

            fntConsole = CreateFontW(fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     ANTIALIASED_QUALITY, FF_DONTCARE, L"Terminal");
            if (fntConsole) {
                ExpandScreenCarpet();

                WelcomeStage();
                if (!exitApp) {
                    DrawSysRead();
                }

                ShrinkScreenCarpet();
            }
        }
    }

    return 0;
}

