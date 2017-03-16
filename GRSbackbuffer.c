#include "GRS.h"

BACKBUFFER *NewBackbuffer(HWND hwnd)
{
    if (!hwnd)
        return (BACKBUFFER*)NULL;

    HDC hdc = GetDC(hwnd);
    if (!hdc)
        return (BACKBUFFER*)NULL;

    BACKBUFFER *new = win_malloc(sizeof(BACKBUFFER));
    if (new == NULL) {
        ReleaseDC(hwnd, hdc);
        return (BACKBUFFER*)NULL;
    }

    GetClientRect(hwnd, &new->rc);

    new->hDC = CreateCompatibleDC(hdc);
    if (!(new->hDC)) {
        win_free(new);
        ReleaseDC(hwnd, hdc);
        return (BACKBUFFER*)NULL;
    }

    new->hBitmap = CreateCompatibleBitmap(hdc, new->rc.right, new->rc.bottom);
    if (!(new->hBitmap)) {
        DeleteDC(new->hDC);
        win_free(new);
        ReleaseDC(hwnd, hdc);
        return (BACKBUFFER*)NULL;
    }

    BitBlt(new->hDC, 0, 0, new->rc.right, new->rc.bottom, hdc, 0, 0, SRCCOPY);
    HGDIOBJ temp = SelectObject(new->hDC, new->hBitmap);
    DeleteObject(temp);
    ReleaseDC(hwnd, hdc);
    return new;
}

void DeleteBackbuffer(BACKBUFFER *to_delete)
{
    if (to_delete != NULL) {
        HGDIOBJ prev = SelectObject(to_delete->hDC, to_delete->hBitmap);

        DeleteObject(prev);
        DeleteObject((HGDIOBJ)to_delete->hBitmap);
        DeleteDC(to_delete->hDC);
        win_free(to_delete);
    }
}

void DrawBufferToWindow(void)
{
    HDC hdc = GetDC(hScreen);

    BitBlt(hdc, 0, 0, screenBuff->rc.right, screenBuff->rc.bottom, screenBuff->hDC, 0, 0, SRCCOPY);
    ReleaseDC(hScreen, hdc);
}
