#include "kcwnotifier.h"

#include <string>
#include <windows.h>
#include "kcwdebug.h"

#define KcwTestAssert(argument, output) if(!(argument)) { OutputDebugString(output); exit(1); }

int main(int argc, char**argv) {
    KcwNotifier notifier1(L"kcwnotifiertest");
    KcwNotifier notifier2(L"kcwnotifiertest");
    KcwTestAssert(notifier1.open() == 0, L"failed to create notification event")
    KcwTestAssert(notifier1.opened(), L"notification event is not opened")
// check whether we can open the event locally at least
    KcwTestAssert(notifier2.open() == 0, L"failed to open notification event")

// test if notification works (locally)
    notifier1.notify();
    DWORD ret = WaitForSingleObject(notifier1.handle(), 1000);
    switch(ret) {
        case WAIT_TIMEOUT: {
            KcwDebug() << "the timeout happened";
            break;
        }
        case WAIT_FAILED: {
            DWORD dw = GetLastError();
            WCHAR buf[1024];
            ZeroMemory(buf, 1024);

            FormatMessageW(
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                buf,
                0, NULL );

            KcwDebug() << "an error occured while waiting for handle" << notifier1.handle() << ":" << (const wchar_t*)buf;
            break;
        }
        case WAIT_OBJECT_0:
        default:
            break;
    };
    KcwTestAssert(ret == WAIT_OBJECT_0, L"the event could not be seen as notified")

    notifier2.close();
    KcwTestAssert(!notifier2.opened(), L"event still opened after calling close")
    return 0;
}