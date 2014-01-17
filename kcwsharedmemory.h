/* Copyright 2013-2014  Patrick Spendrin <ps_ml@gmx.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef sharedmemory_h
#define sharedmemory_h

#include <string>
#include <cstdio>
#include <windows.h>
#include <cassert>

/**
* @author Patrick Spendrin
* @date 2011-2013
* @brief This class is used to abstract a shared memory object
* @details Shared memory can be used for interprocess communication. For this purpose
* a KcwSharedMemory object is created on both sides, on one of them create() is called,
* on the other one you call open(). After that, you can use the memory objects on both
* sides similar to a pointer to a normal object.
*/
template<typename T>
class KcwSharedMemory {
    public:
        KcwSharedMemory();
        KcwSharedMemory(const std::wstring& strName, int size = 1, bool bCreate = true);

        ~KcwSharedMemory();

        /**@{*/
        /**
        * create a new shared memory space with the name @p strName and @p size. In case that a shared memory
        * space is already opened in this object, do not create a new one but simply return 0.
        * @return 0 in case a shared memory could be added and a value lower than 0 in case something
        * went wrong.
        */
        inline int create(const std::wstring& strName, int size = 1);

        /**
        * open an existing shared memory space with the name @p strName. In case that a shared memory
        * space is already opened in this object, do not create a new one but simply return 0.
        * @return 0 in case a shared memory could be added and a value lower than 0 in case something
        * went wrong.
        */
        inline int open(const std::wstring& strName);

        /**
         * closes this shared memory space. after that, the memory can be reopened with open.
         * @return true in case the memory could be closed successfully and false in case something
         * went wrong.
         */
        inline bool close();

        /**
         * resizes the current object.
         */
        int resize(int size);
        /**@}*/

        /**
        * An internal convenience function to output the last error that occured and exit the application.
        */
        inline void errorExit();


        inline T& operator[](size_t index) const;
        inline T* operator->() const;
        inline T& operator*() const;
        inline KcwSharedMemory& operator=(const T& val);

        /**
        * @return a pointer to the shared memory segment.
        */
        inline T* data();

        /**
         * @return a const pointer to the shared memory segment.
         */
        inline T* const data() const;

        /**
        * @return the size of this shared memory segment.
        */
        int size() const;

        /**
        * @return whether this shared memory segment has been opened already.
        * Note that a segment of size 0 is never opened.
        */
        bool opened() const;

        /**
         * @return the name of the shared memory segment
         */
        std::wstring name() const;
    private:
        std::wstring m_name;
        int         m_size;
        HANDLE      m_sharedMemHandle;
        T*          m_sharedMem;
};

/**
* constructs a default object of Type T. The object cannot be used before calling
* either open() or create().
*/
template<typename T>
KcwSharedMemory<T>::KcwSharedMemory()
: m_name(L""),
  m_size(0),
  m_sharedMem(NULL),
  m_sharedMemHandle(NULL) {
}


/**
* constructs an object with the name @p strName; If you want to just open the shared memory
* add @p bCreate.
* This is only available for convenience, it is equivalent to construction of a default and
* KcwSharedMemory object and calling open() or create() afterwards.
*/
template<typename T>
KcwSharedMemory<T>::KcwSharedMemory(const std::wstring& strName, int size, bool bCreate)
: m_name(strName),
  m_size(size),
  m_sharedMem(NULL),
  m_sharedMemHandle(NULL) {
    if (bCreate)
    {
        create(strName, size);
    }
    else
    {
        open(strName);
    }
}

template<typename T>
KcwSharedMemory<T>::~KcwSharedMemory() {
    WCHAR buf[1024];
    ZeroMemory(buf, 1024);

    wsprintf(buf, L"deleting shared memory object %s", m_name.c_str());
//     OutputDebugStringW(buf);
    close();
}

template<typename T>
void KcwSharedMemory<T>::errorExit() {
    WCHAR buf[1024];
    ZeroMemory(buf, 1024);
    DWORD dw = GetLastError();

    FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buf,
        0, NULL );

    wsprintf(buf, L"sharedMemory %s: an error with id %i happened: %s", m_name.c_str(), dw, buf);
    OutputDebugStringW(buf);
    ExitProcess(dw);
}

template<typename T>
int KcwSharedMemory<T>::create(const std::wstring& strName, int size) {
    m_name      = strName;
    m_size      = size;

    // if the file handle already is set, expect it to be set correctly and don't reopen it
    if(m_sharedMemHandle != NULL) return 0;

    m_sharedMemHandle = ::CreateFileMapping(INVALID_HANDLE_VALUE,
                                            NULL,
                                            PAGE_EXECUTE_READWRITE,
                                            0,
                                            m_size * sizeof(T) + sizeof(m_size),
                                            m_name.c_str());

    if(m_sharedMemHandle == NULL) {
        return -1;
    }

    m_sharedMem = static_cast<T*>(::MapViewOfFile(m_sharedMemHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0));
    if(m_sharedMem == NULL) {
        return -2;
    }

    ::ZeroMemory(m_sharedMem, m_size * sizeof(T) + sizeof(m_size));
    memcpy(m_sharedMem, &m_size, sizeof(m_size));
    return 0;
}

template<typename T>
int KcwSharedMemory<T>::open(const std::wstring& strName) {
    m_name   = strName;

    // if the file handle already is set, expect it to be set correctly and don't reopen it
    if(m_sharedMemHandle != NULL) return 0;

//    OutputDebugStringA((std::wstring("key: ").append(m_name)).c_str());

    m_sharedMemHandle = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_name.c_str());

    if (!m_sharedMemHandle || (m_sharedMemHandle == INVALID_HANDLE_VALUE)) {
        return -1;
    }

    m_sharedMem = static_cast<T*>(::MapViewOfFile(m_sharedMemHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0));

    if (!m_sharedMem) {
        return -2;
    }
    memcpy(&m_size, m_sharedMem, sizeof(m_size));
    return 0;
}

template<typename T>
bool KcwSharedMemory<T>::close() {
    // not implemented yet
    UnmapViewOfFile((LPCVOID)m_sharedMem);
    CloseHandle(m_sharedMemHandle);
    m_sharedMem = NULL;
    m_sharedMemHandle = NULL;
    m_size = 0;
    return true;
}

template<typename T>
int KcwSharedMemory<T>::resize(int size) {
    if(m_sharedMemHandle == NULL) return -1;

    T* safeBuffer = new T[m_size];
    memcpy(safeBuffer, m_sharedMem + sizeof(m_size), m_size * sizeof(T));
    int oldsize = m_size;
    close();
    int res = create(m_name, size);
    if(res != 0) return res;
    memcpy(m_sharedMem + sizeof(m_size), safeBuffer, ((oldsize < size) ? oldsize : size) * sizeof(T));
    return 0;
}

template<typename T>
T* KcwSharedMemory<T>::operator->() const {
    assert(m_size);
    assert(m_sharedMem);
    return m_sharedMem + sizeof(m_size);
}

template<typename T>
T& KcwSharedMemory<T>::operator*() const {
    assert(m_size);
    assert(m_sharedMem);
    return *(m_sharedMem + sizeof(m_size));
}

template<typename T>
KcwSharedMemory<T>& KcwSharedMemory<T>::operator=(const T& val) {
    assert(m_size);
    assert(m_sharedMem);
    *(m_sharedMem + sizeof(m_size)) = val;
    return *this;
}

template<typename T>
T& KcwSharedMemory<T>::operator[](size_t index) const {
    assert(m_size);
    assert(m_sharedMem);
    return *(m_sharedMem + sizeof(m_size) + index * sizeof(T));
}

template<typename T>
T* KcwSharedMemory<T>::data() {
    assert(m_size);
    assert(m_sharedMem);
    return m_sharedMem + sizeof(m_size);
}

template<typename T>
T* const KcwSharedMemory<T>::data() const {
    assert(m_size);
    assert(m_sharedMem);
    return m_sharedMem + sizeof(m_size);
}

template<typename T>
int KcwSharedMemory<T>::size() const {
    return m_size;
}

template<typename T>
bool KcwSharedMemory<T>::opened() const {
    return (m_size != 0);
}

template<typename T>
std::wstring KcwSharedMemory<T>::name() const {
    return m_name;
}
#endif /* sharedmemory_h */
