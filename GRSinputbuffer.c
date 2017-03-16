#include "GRS.h"


WCHAR szInputBuff[MAX_COLUMN_SZ] = {L'_'};
uint32_t current_column = 1;

static uint32_t curr_idx = 0;

/* returns TRUE if the RETURN key was pressed */
BOOL ReadNextCharToInputBuffer(void)
{
    BOOL bRet = FALSE;

    EnterCriticalSection(&csReadKeystroke);
    if (g_keystroke.key_is_pressed && g_keystroke.is_printable) {

        switch (g_keystroke.pressed_char) {
            case VK_ESCAPE:
                exitApp = TRUE;
                break;
            case VK_BACK:
                if (curr_idx > 0)
                    curr_idx--;
                szInputBuff[curr_idx] = L'_';
                break;
            case VK_RETURN:
                bRet = TRUE;
                break;
            default:
                if (curr_idx >= MAX_COLUMN_SZ)
                    break;//curr_idx = 0; only support one row for now

                szInputBuff[curr_idx++] = g_keystroke.pressed_char;
                szInputBuff[curr_idx] = L'_';
                break;
        }

        current_column = curr_idx + 1;
        g_keystroke.key_is_pressed = FALSE;
    }
    LeaveCriticalSection(&csReadKeystroke);

    return bRet;
}

BOOL StrCmpInputBuffer(WCHAR *szBuff)
{
    int i, z = MAX_COLUMN_SZ - 1;

    for (
         i = 0;
         (
          (szBuff[i] != L'\0')
          &&
          (i < z)
          &&
          (szInputBuff[i] != L'_')
          &&
          (szBuff[i] == szInputBuff[i])
         );
         i++
        );

    return (szBuff[i] == L'\0');
}
