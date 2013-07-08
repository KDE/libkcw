#include "kcwapp.h"
#include "kcweventloop.h"
#include "kcwdebug.h"

static KcwApp* kcwApp = 0;
KcwApp::KcwApp() : KcwEventLoop() {
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)&KcwApp::HandlerRoutine, TRUE);
    kcwApp = this;
}

void KcwApp::quit() {
    KcwEventLoop::quit();
}

BOOL KcwApp::HandlerRoutine(DWORD dwCtrlType) {
    BOOL retval = FALSE;
    switch(dwCtrlType) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:  {
                                    KcwDebug() << "Ctrl+c or Ctrl+Break pressed!";
//                                     kcwApp->quit();
                                    retval = TRUE;
                                    break;
                                }
        case CTRL_CLOSE_EVENT:
        default: break;
    };
    return retval;
}

