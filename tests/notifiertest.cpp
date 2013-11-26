#include "kcwnotifier.h"

#include <string>
#include <locale>
#include <windows.h>
#include "kcwdebug.h"
#include "kcwprocess.h"

#define KcwTestAssert(argument, output) if(!(argument)) { OutputDebugString(output); exit(1); }

int main(int argc, char**argv) {
    if(argc == 2) {
        KcwDebug() << "secondary process opened!";
        KcwNotifier remotenotifier1(L"remotekcwnotifiertest");
        KcwNotifier remotenotifier2(L"remotekcwnotifiertest2");
        if(remotenotifier1.open() != 0) {
            KcwDebug() << L"failed to open remote notifier1";
            return -1;
        }

        if(remotenotifier2.open() != 0) {
            KcwDebug() << L"failed to open remote notifier2";
            return -2;
        }

        remotenotifier1.notify();
        DWORD result = WaitForSingleObject(remotenotifier2.handle(), 1000);
        if(result != WAIT_OBJECT_0) return -3;
        return 0;
    }

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

// testing access another process
    KcwNotifier notifier3(L"remotekcwnotifiertest");
    KcwTestAssert(notifier3.open() == 0, L"could not create notifier event")

    std::string arg0 = argv[0];
    wchar_t* w = new wchar_t[arg0.size() + 1];
    std::locale loc;
    std::use_facet< std::ctype<wchar_t> >(loc).widen (arg0.data(), arg0.data() + arg0.size() + 1, w);
    KcwProcess proc(std::wstring(w) + L" runcheck1");
    KcwTestAssert(proc.start(), L"couldn't start process in paused mode")
    KcwTestAssert(proc.resume(), L"couldn't resume process from paused mode")
    ret = WaitForSingleObject(notifier3.handle(), 1000);
    KcwTestAssert(ret == WAIT_OBJECT_0, L"notification wasn't received")
    KcwNotifier notifier4(L"remotekcwnotifiertest2");
    KcwTestAssert(notifier4.open() == 0, L"couldn't open remote notifier event")
    notifier4.notify();
    ret = WaitForSingleObject(proc.process(), 1000);
    KcwTestAssert(ret == WAIT_OBJECT_0, L"couldn't wait for external process")
    KcwTestAssert(proc.exitCode() == 0, L"some problem in the external process occured")
    return 0;
}