#include <string>
#include <windows.h>

#include "kcwsharedmemory.h"
#include "kcwdebug.h"

#define KcwTestAssert(argument, output) if(!(argument)) { OutputDebugString(output); exit(1); }

int main(int argc, char ** argv) {
    KcwSharedMemory<char> test1;
    KcwSharedMemory<char> test2;

    test1.create(L"sharedmemorytest", 123);
    test2.open(L"sharedmemorytest");
    KcwTestAssert(test1.size() == 123, L"created shared memory segment has wrong size");
    KcwTestAssert(test1.size() == 123, L"opened shared memory segment has wrong size");
    KcwTestAssert(test1.opened() == true, L"shared memory segment is not opened!");

    CopyMemory(test1.data(), "This is a very long string, long enough to be bigger than 123 characters (some characters"
                             " more than the size of the shared memory segment!).", 123);
    test1[122] = 0;
    KcwTestAssert((memcmp(test1.data(), "This is a very long string, long enough to be bigger than 123 characters (some characters"
                                        " more than the size of the shared", 123) == 0),
                                        L"written data in the source shared memory object is broken");
    KcwTestAssert((memcmp(test2.data(), "This is a very long string, long enough to be bigger than 123 characters (some characters"
                                        " more than the size of the shared", 123) == 0),
                                        L"written data in the source shared memory object is broken");

    KcwSharedMemory<int> test3;
    KcwSharedMemory<int> test4;

    test3.create(L"sharedmemorytest-int", 10);
    test4.open(L"sharedmemorytest-int");
    KcwTestAssert(test3.opened() == true, L"shared memory segment is not opened!");
    KcwTestAssert(test4.opened() == true, L"shared memory segment is not opened!");
    for(int i = 0; i < 10; i++) test3[i] = i;
    for(int i = 0; i < 10; i++) KcwTestAssert((test4[i] == i), L"indexes don't work correctly");

    test3.close();
    KcwTestAssert(test3.opened() == false, L"shared memory segment is not closed correctly");
    KcwTestAssert(test4.opened() == true, L"shared memory segment closed although it should stay open");

    KcwTestAssert(test3.open(L"sharedmemorytest-int") == 0, L"shared memory segment could not be reopened");
    test3.close();
    test4.close();
    KcwTestAssert(test3.open(L"sharedmemorytest-int") != 0, L"shared memory segment shouldn't be open anymore");
    KcwTestAssert(test3.opened() == false, L"shared memory segment should not be open now");

    KcwSharedMemory<int> test5;
    test5.open(L"sharedmemorytest-fail");
    KcwTestAssert(test5.opened() == false, L"shared memory segment exists even though it shouldn't!");
    return 0;
}