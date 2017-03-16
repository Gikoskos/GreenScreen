#include "GRS.h"

HANDLE hDataRprThread = NULL;
CRITICAL_SECTION csReadUserData;
USER User = {
    .szName = NULL,
    .szNameUpper = NULL
};

static UINT CALLBACK DataRprThread(PVOID args);
static void GetThrdOwnerUserName(void);


UINT CALLBACK DataRprThread(PVOID args)
{
    UNREFERENCED_PARAMETER(args);

    GetThrdOwnerUserName();

    _endthreadex(0);
}

void GetThrdOwnerUserName(void)
{
    DWORD len = 0;

    GetUserNameW(NULL, &len);

    EnterCriticalSection(&csReadUserData);
    if (!(User.szName = win_malloc((len + 1) * sizeof(WCHAR))))
        exit(EXIT_FAILURE);

    if (!GetUserNameW(User.szName, &len)) {
        LeaveCriticalSection(&csReadUserData);
        exit(EXIT_FAILURE);
    }

    User.szName[len] = L'\0';

    if (!(User.szNameUpper = win_malloc(sizeof(WCHAR) * (len + 1))))
        exit(EXIT_FAILURE);

    if (FAILED(StringCchCopyNW(User.szNameUpper, len + 1, User.szName, len + 1)))
        exit(EXIT_FAILURE);

    CharUpperW(User.szNameUpper);

    LeaveCriticalSection(&csReadUserData);
}

void InitDataReaperThread(void)
{
    InitializeCriticalSection(&csReadUserData);

    hDataRprThread = (HANDLE)_beginthreadex(NULL, 0, DataRprThread, (PVOID)0, 0, NULL);
    if (!hDataRprThread)
        exit(EXIT_FAILURE);
}
