#include "GRS.h"


static void WelcomeMsgAnimate(void);
static BOOL WelcomeMsgWaitInput(void);
static void WelcomeRightAnimate(void);
static void WelcomeWrongAnimate(void);

/* STAGE 1 */
void WelcomeStage(void)
{
    WelcomeMsgAnimate();
    if (WelcomeMsgWaitInput()) {
        if (!exitApp)
            WelcomeRightAnimate();
    } else {
        if (!exitApp)
            WelcomeWrongAnimate();
    }
}

BOOL WelcomeMsgWaitInput(void)
{
    int firstCol, firstRow;
    HFONT fntPrev = SelectFont(screenBuff->hDC, fntConsole);
    WCHAR szPrintMsg[] = L"WELCOME FRIEND. WHAT IS YOUR NAME?";
    int cx, cy;
    BOOL q_answered = FALSE;

    cx = screenBuff->rc.right/3;
    cy = screenBuff->rc.bottom/6;
    firstCol = screenBuff->rc.right/4;
    firstRow = screenBuff->rc.bottom/4;
    SetBkColor(screenBuff->hDC, RGB_PURPLEBLUE);

    while (!exitApp) {
        //HGDIOBJ hPrevObj;
        HBRUSH hBrush = CreateSolidBrush(RGB_PURPLEBLUE);

        PatBlt(screenBuff->hDC, 0, 0, screenBuff->rc.right, screenBuff->rc.bottom, BLACKNESS);
        FillRect(screenBuff->hDC, &screenBuff->rc, hBrush);

        SetTextColor(screenBuff->hDC, RGB_BLINKING);
        TextOutW(screenBuff->hDC, cx, cy, szPrintMsg, ARRAYSIZE(szPrintMsg));


        if (ReadNextCharToInputBuffer()) {

            EnterCriticalSection(&csReadUserData);
            if (!User.szNameUpper) {
                LeaveCriticalSection(&csReadUserData);
                continue;
            }

            if (StrCmpInputBuffer(User.szNameUpper)) {
                q_answered = TRUE;
            }

            LeaveCriticalSection(&csReadUserData);
            break;
        }

        SetTextColor(screenBuff->hDC, RGB_GREEN);
        TextOutW(screenBuff->hDC, firstCol, firstRow, szInputBuff, current_column);

        DeleteObject((HGDIOBJ)hBrush);

        DrawBufferToWindow();
    }

    SelectFont(screenBuff->hDC, fntPrev);

    return q_answered;
}

void WelcomeWrongAnimate(void)
{
    uint64_t anim_idx = 0, anim_counter = 0;
    const uint64_t animation_timer = 10;

    WCHAR szPrintMsg[] = L"NICE NAME BUT I'D RATHER CALL YOU", *szUserName;
    int anim_stage = 0;
    size_t UserName_len;
    int cxPrintMsg, cyPrintMsg, cxName, cyName, retries = 0;
    HFONT fntPrev = SelectFont(screenBuff->hDC, fntConsole);

    cxPrintMsg = CENTER_CX_SCREEN(ARRAYSIZE(szPrintMsg));
    cyPrintMsg = screenBuff->rc.bottom/6;

RETRY:
    EnterCriticalSection(&csReadUserData);
    if (!User.szNameUpper) {
        LeaveCriticalSection(&csReadUserData);

        if (retries++ > 5)
            exit(EXIT_FAILURE);
        Sleep(100);
        goto RETRY;
    }

    if (FAILED(StringCchLength(User.szNameUpper, UNLEN + 1, &UserName_len)))
        exit(EXIT_FAILURE);

    if (!(szUserName = win_malloc(sizeof(WCHAR) * (UserName_len + 1))))
        exit(EXIT_FAILURE);

    if (FAILED(StringCchCopyNW(szUserName, UserName_len + 1, User.szNameUpper, UserName_len + 1)))
        exit(EXIT_FAILURE);
    LeaveCriticalSection(&csReadUserData);

    cxName = CENTER_CX_SCREEN(UserName_len);
    cyName = cyPrintMsg + 32;

    SetBkColor(screenBuff->hDC, RGB_PURPLEBLUE);
    while (!exitApp) {
        //HGDIOBJ hPrevObj;
        HBRUSH hBrush = CreateSolidBrush(RGB_PURPLEBLUE);

        PatBlt(screenBuff->hDC, 0, 0, screenBuff->rc.right, screenBuff->rc.bottom, BLACKNESS);
        FillRect(screenBuff->hDC, &screenBuff->rc, hBrush);

        ReadNextCharToInputBuffer();

        anim_counter++;

        SetTextColor(screenBuff->hDC, RGB_BLINKING);
        if (!anim_stage) {
            if (!(anim_counter%animation_timer))
                anim_idx++;

            if (anim_idx > ARRAYSIZE(szPrintMsg)) {
                anim_idx = 0;
                anim_stage++;
            }

            TextOutW(screenBuff->hDC, cxPrintMsg, cyPrintMsg, szPrintMsg, anim_idx);
        } else if (anim_stage == 1) {
            if (!(anim_idx > UserName_len) && !(anim_counter%animation_timer))
                anim_idx++;
            else if (anim_idx > UserName_len)
                anim_stage++;

            TextOutW(screenBuff->hDC, cxPrintMsg, cyPrintMsg, szPrintMsg, ARRAYSIZE(szPrintMsg));
            TextOutW(screenBuff->hDC, cxName, cyName, szUserName, anim_idx);
        } else {
            TextOutW(screenBuff->hDC, cxPrintMsg, cyPrintMsg, szPrintMsg, ARRAYSIZE(szPrintMsg));
            TextOutW(screenBuff->hDC, cxName, cyName, szUserName, UserName_len);

            if (anim_counter > 800)
                break;
        }

        DeleteObject((HGDIOBJ)hBrush);

        DrawBufferToWindow();
    }

    win_free(szUserName);
    SelectObject(screenBuff->hDC, fntPrev);
}

void WelcomeRightAnimate(void)
{
    uint64_t anim_idx = 0, anim_counter = 0;
    const uint64_t animation_timer = 10;

    WCHAR *szPrintMsg, *szUserName;
    size_t PrintMsg_len;
    int cx, cy, retries = 0;
    HFONT fntPrev = SelectFont(screenBuff->hDC, fntConsole);

    cy = screenBuff->rc.bottom/6;

RETRY:
    EnterCriticalSection(&csReadUserData);
    if (User.szNameUpper) {
        if (FAILED(StringCchLengthW(User.szNameUpper, UNLEN + 1, &PrintMsg_len)))
            exit(EXIT_FAILURE);

        if (!(szUserName = win_malloc(sizeof(WCHAR) * (PrintMsg_len + 1))))
            exit(EXIT_FAILURE);

        StringCchCopyNW(szUserName, PrintMsg_len + 1, User.szNameUpper, PrintMsg_len + 1);
    } else {
        LeaveCriticalSection(&csReadUserData);
        if (retries++ > 5)
            exit(EXIT_FAILURE);

        Sleep(100);
        goto RETRY;
    }
    LeaveCriticalSection(&csReadUserData);

    PrintMsg_len += 31;
    if (!(szPrintMsg = win_malloc(sizeof(WCHAR) * PrintMsg_len)))
        exit(EXIT_FAILURE);

    StringCchPrintfW(szPrintMsg, PrintMsg_len, L"NICE NAME %s, LIKE YOUR USERNAME", szUserName);
    cx = CENTER_CX_SCREEN(PrintMsg_len);

    SetBkColor(screenBuff->hDC, RGB_PURPLEBLUE);

    while (!exitApp) {
        //HGDIOBJ hPrevObj;
        HBRUSH hBrush = CreateSolidBrush(RGB_PURPLEBLUE);

        PatBlt(screenBuff->hDC, 0, 0, screenBuff->rc.right, screenBuff->rc.bottom, BLACKNESS);
        FillRect(screenBuff->hDC, &screenBuff->rc, hBrush);

        ReadNextCharToInputBuffer();

        anim_counter++;
        if (!(anim_idx > PrintMsg_len) && !(anim_counter%animation_timer))
            anim_idx++;
        else if (anim_idx > PrintMsg_len)
            if (anim_counter > 800)
                break;

        SetTextColor(screenBuff->hDC, RGB_BLINKING);
        TextOutW(screenBuff->hDC, cx, cy, szPrintMsg, anim_idx);

        DeleteObject((HGDIOBJ)hBrush);

        DrawBufferToWindow();
    }

    win_free(szUserName);
    win_free(szPrintMsg);
    SelectObject(screenBuff->hDC, fntPrev);
}

void WelcomeMsgAnimate(void)
{
    uint64_t anim_idx = 0, anim_counter = 0;
    const uint64_t animation_timer = 10;

    WCHAR szPrintMsg[] = L"WELCOME FRIEND. WHAT IS YOUR NAME?";
    int cx, cy;
    HFONT fntPrev = SelectFont(screenBuff->hDC, fntConsole);

    cx = screenBuff->rc.right/3;
    cy = screenBuff->rc.bottom/6;
    SetBkColor(screenBuff->hDC, RGB_PURPLEBLUE);

    while (!exitApp) {


        //HGDIOBJ hPrevObj;
        HBRUSH hBrush = CreateSolidBrush(RGB_PURPLEBLUE);

        PatBlt(screenBuff->hDC, 0, 0, screenBuff->rc.right, screenBuff->rc.bottom, BLACKNESS);
        FillRect(screenBuff->hDC, &screenBuff->rc, hBrush);

        ReadNextCharToInputBuffer();

        anim_counter++;
        if (!(anim_counter%animation_timer))
            anim_idx++;

        if (anim_idx > ARRAYSIZE(szPrintMsg))
            break;

        SetTextColor(screenBuff->hDC, RGB_BLINKING);
        TextOutW(screenBuff->hDC, cx, cy, szPrintMsg, anim_idx);

        DeleteObject((HGDIOBJ)hBrush);

        DrawBufferToWindow();
    }

    SelectObject(screenBuff->hDC, fntPrev);
}

void ExpandScreenCarpet(void)
{
    POINT ptCarpet[] = {{0,0}, {0,0}, {0,0}, {0,0}};
    BOOL screen_is_filled = FALSE;
    int expansion_delta = 5;
    HBRUSH hBrush = CreateSolidBrush(RGB_PURPLEBLUE);
    HGDIOBJ hPrevBrush = SelectObject(screenBuff->hDC, hBrush);

    while (!screen_is_filled) {
        screen_is_filled = TRUE;

        if (ptCarpet[1].y < screenBuff->rc.bottom) {
            ptCarpet[1].y += expansion_delta;
            screen_is_filled = FALSE;
        }

        if (ptCarpet[2].x < screenBuff->rc.right) {
            ptCarpet[2].x += 1.2*expansion_delta;
            screen_is_filled = FALSE;
        }

        if (ptCarpet[2].y < screenBuff->rc.bottom) {
            ptCarpet[2].y += 1.5*expansion_delta;
            screen_is_filled = FALSE;
        }

        if (ptCarpet[3].x < screenBuff->rc.right) {
            ptCarpet[3].x += expansion_delta;
            screen_is_filled = FALSE;
        }

        Polygon(screenBuff->hDC, ptCarpet, 4);
        DrawBufferToWindow();
    }

    SelectObject(screenBuff->hDC, hPrevBrush);
    DeleteObject((HGDIOBJ)hBrush);
}

void ShrinkScreenCarpet(void)
{
    POINT ptCarpet[] = {
                        {0, 0},
                        {0, screenBuff->rc.bottom},
                        {screenBuff->rc.right, screenBuff->rc.bottom},
                        {screenBuff->rc.right, 0}
    };
    int cxScreenMedian = screenBuff->rc.right/2, cyScreenMedian = screenBuff->rc.bottom/2;
    BOOL carpet_is_shrinked = FALSE;
    int shrinking_delta = 5;
    //BLENDFUNCTION blend = { AC_SRC_OVER, 0, 0, 0 };
    HBRUSH hBrush = CreateSolidBrush(RGB_PURPLEBLUE);
    HGDIOBJ hPrevBrush = SelectObject(screenBuff->hDC, hBrush);

    while (!carpet_is_shrinked) {
        carpet_is_shrinked = TRUE;

        if (ptCarpet[0].x < cxScreenMedian) {
            ptCarpet[0].x += shrinking_delta;
            carpet_is_shrinked = FALSE;
        }

        if (ptCarpet[0].y < cyScreenMedian) {
            ptCarpet[0].y += shrinking_delta;
            carpet_is_shrinked = FALSE;
        }

        if (ptCarpet[1].x < cxScreenMedian) {
            ptCarpet[1].x += shrinking_delta;
            carpet_is_shrinked = FALSE;
        }

        if (ptCarpet[1].y > cyScreenMedian) {
            ptCarpet[1].y -= shrinking_delta;
            carpet_is_shrinked = FALSE;
        }

        if (ptCarpet[2].x > cxScreenMedian) {
            ptCarpet[2].x -= shrinking_delta;
            carpet_is_shrinked = FALSE;
        }

        if (ptCarpet[2].y > cyScreenMedian) {
            ptCarpet[2].y -= shrinking_delta;
            carpet_is_shrinked = FALSE;
        }

        if (ptCarpet[3].x > cxScreenMedian) {
            ptCarpet[3].x -= shrinking_delta;
            carpet_is_shrinked = FALSE;
        }

        if (ptCarpet[3].y < cyScreenMedian) {
            ptCarpet[3].y += shrinking_delta;
            carpet_is_shrinked = FALSE;
        }

        PatBlt(screenBuff->hDC, 0, 0, screenBuff->rc.right, screenBuff->rc.bottom, BLACKNESS);

        Polygon(screenBuff->hDC, ptCarpet, 4);
        DrawBufferToWindow();
    }

    SelectObject(screenBuff->hDC, hPrevBrush);
    DeleteObject((HGDIOBJ)hBrush);
}
