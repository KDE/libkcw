#include "kcwapp.h"
#include "kcwdebug.h"

#include <windows.h>

#include "simpleinjectable.h"

#define KcwTestAssert(argument, output) if(!(argument)) { OutputDebugString(output); exit(1); }

std::wstring getModulePath(HMODULE hModule) {
    WCHAR szModulePath[MAX_PATH + 1];
    ::ZeroMemory(szModulePath, (MAX_PATH + 1)*sizeof(WCHAR));

    ::GetModuleFileName(hModule, szModulePath, MAX_PATH);

    std::wstring strPath(szModulePath);

    return strPath.substr(0, strPath.rfind('\\'));
}


int main(int argc, char**argv) {
    KcwApp app;
//     HANDLE timer = CreateWaitableTimer(NULL, FALSE, NULL);
    int retval = 0;

//     LARGE_INTEGER li;
//     li.QuadPart = -30 * 1000000LL; // 2 seconds
//     SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE);
//     app.addCallback(timer, CB(KcwApp::quit));
//     retval = app.exec();
    startConsole();
    return retval;
}