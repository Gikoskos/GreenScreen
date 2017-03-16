#include "GRS.h"

#define FIXED_BUFF 100

void DrawSysRead(void)
{
    uint64_t anim_idx = 0, anim_counter = 0;
    const uint64_t animation_timer = 10;

    PC sysinf;
    WCHAR szBrowserMsg[] = L"DETECTED BROWSERS: ",
          szArchMsg[] = L"ARCHITECTURE: ", szBrowser[FIXED_BUFF], szArch[FIXED_BUFF];
    int cxBrowserMsg, cxArchMsg, cxBrowser, cxArch, cyBrowserMsg, cyArchMsg, cyBrowser, cyArch;
    int Browser_len, Arch_len;
    int anim_stage = 0;

    HBRUSH hBrush;
    HFONT fntPrev = SelectFont(screenBuff->hDC, fntConsole);
   
    EnterCriticalSection(&csReadPCData);
    sysinf = this_system;
    LeaveCriticalSection(&csReadPCData);

    switch (sysinf.arch) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            Arch_len = 19;
            StringCchPrintfW(szArch, Arch_len, L"x64 (AMD OR INTEL)");
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            Arch_len = 4;
            StringCchPrintfW(szArch, Arch_len, L"ARM");
            break;
        case PROCESSOR_ARCHITECTURE_IA64:
            Arch_len = 20;
            StringCchPrintfW(szArch, Arch_len, L"INTEL ITANIUM-BASED");
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            Arch_len = 4;
            StringCchPrintfW(szArch, Arch_len, L"x86");
            break;
        case PROCESSOR_ARCHITECTURE_UNKNOWN:
            Arch_len = 21;
            StringCchPrintfW(szArch, Arch_len, L"UNKNOWN ARCHITECTURE");
            break;
        default:
            exit(EXIT_FAILURE);
    }

    if (FIREFOX_BROWSER & sysinf.brInstalled) {
        Browser_len = 16;
        StringCchPrintfW(szBrowser, Browser_len, L"MOZILLA FIREFOX");
    } else {
        Browser_len = 18;
        StringCchPrintfW(szBrowser, Browser_len, L"NO KNOWN BROWSERS");
    }

    cxBrowserMsg = CENTER_CX_SCREEN(ARRAYSIZE(szBrowserMsg))/2;
    cyBrowserMsg = screenBuff->rc.bottom/6;
    cxBrowser = CENTER_CX_SCREEN(Browser_len);
    cyBrowser = cyBrowserMsg + fontSize + 4;
    cxArchMsg = CENTER_CX_SCREEN(ARRAYSIZE(szArchMsg))/2;
    cyArchMsg = cyBrowser + fontSize + 4;
    cxArch = CENTER_CX_SCREEN(Arch_len);
    cyArch = cyArchMsg + fontSize + 4;

    SetBkColor(screenBuff->hDC, RGB_PURPLEBLUE);

    while (!exitApp) {
        //HGDIOBJ hPrevObj;
        hBrush = CreateSolidBrush(RGB_PURPLEBLUE);

        PatBlt(screenBuff->hDC, 0, 0, screenBuff->rc.right, screenBuff->rc.bottom, BLACKNESS);
        FillRect(screenBuff->hDC, &screenBuff->rc, hBrush);

        SetTextColor(screenBuff->hDC, RGB_BLINKING);

        ReadNextCharToInputBuffer();

        anim_counter++;
        if (!(anim_counter%animation_timer))
            anim_idx++;


        switch (anim_stage) {
            case 0:
                if (anim_idx > ARRAYSIZE(szBrowserMsg)) {
                    anim_idx = 0;
                    anim_stage++;
                }
                TextOutW(screenBuff->hDC, cxBrowserMsg, cyBrowserMsg, szBrowserMsg, anim_idx);
                break;
            case 1:
                if (anim_idx > Browser_len) {
                    anim_idx = 0;
                    anim_stage++;
                }
                TextOutW(screenBuff->hDC, cxBrowserMsg, cyBrowserMsg, szBrowserMsg, ARRAYSIZE(szBrowserMsg));
                TextOutW(screenBuff->hDC, cxBrowser, cyBrowser, szBrowser, anim_idx);
                break;
            case 2:
                if (anim_idx > ARRAYSIZE(szArchMsg)) {
                    anim_idx = 0;
                    anim_stage++;
                }
                TextOutW(screenBuff->hDC, cxBrowserMsg, cyBrowserMsg, szBrowserMsg, ARRAYSIZE(szBrowserMsg));
                TextOutW(screenBuff->hDC, cxBrowser, cyBrowser, szBrowser, Browser_len);
                TextOutW(screenBuff->hDC, cxArchMsg, cyArchMsg, szArchMsg, anim_idx);
                break;
            case 3:
                if (anim_idx > Arch_len) {
                    anim_idx = 0;
                    anim_stage++;
                }
                TextOutW(screenBuff->hDC, cxBrowserMsg, cyBrowserMsg, szBrowserMsg, ARRAYSIZE(szBrowserMsg));
                TextOutW(screenBuff->hDC, cxBrowser, cyBrowser, szBrowser, Browser_len);
                TextOutW(screenBuff->hDC, cxArchMsg, cyArchMsg, szArchMsg, ARRAYSIZE(szArchMsg));
                TextOutW(screenBuff->hDC, cxArch, cyArch, szArch, anim_idx);
                break;
            case 4:
                TextOutW(screenBuff->hDC, cxBrowserMsg, cyBrowserMsg, szBrowserMsg, ARRAYSIZE(szBrowserMsg));
                TextOutW(screenBuff->hDC, cxBrowser, cyBrowser, szBrowser, Browser_len);
                TextOutW(screenBuff->hDC, cxArchMsg, cyArchMsg, szArchMsg, ARRAYSIZE(szArchMsg));
                TextOutW(screenBuff->hDC, cxArch, cyArch, szArch, Arch_len);
                if (anim_counter > 3000)
                    exitApp = TRUE;
                break;
        }

        DeleteObject((HGDIOBJ)hBrush);

        DrawBufferToWindow();
    }

    SelectObject(screenBuff->hDC, fntPrev);
}
