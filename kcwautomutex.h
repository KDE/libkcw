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
#ifndef kcwautomutex_h
#define kcwautomutex_h

#include <windows.h>
#include "kcwdebug.h"

/**
* @author Patrick Spendrin
* @date 2011-2013
*
* @brief KcwEventLoop is the event loop base class which handles the event distribution.
*
* If you want to override the default application wide event handle for this loop, 
* give it as parameter or set it before calling exec().
*/

struct KcwAutoMutex
{
    HANDLE m_mutex;
    char m_func[256];

    KcwAutoMutex(HANDLE mutex) : m_mutex(mutex)
    {
    }

    bool lock(const char* f = NULL, int timeout=1000)
    {
        if(f != NULL && strlen(f) > 0) memcpy(m_func, f, sizeof(m_func) - 1);
        else ZeroMemory(m_func, sizeof(m_func));
        m_func[sizeof(m_func) - 1] = 0;
//         if(m_func[0] != 0) KcwDebug() << m_func << "enter mutex!";
        if(WaitForSingleObject(m_mutex, timeout) != WAIT_OBJECT_0) {
            KcwDebug() << "mutex for" << m_func << "failed!";
            return false;
        }
        return true;
    }

    bool unlock()
    {
        ReleaseMutex(m_mutex);
//         if(m_func[0] != 0) KcwDebug() << m_func << "release mutex!";
        return true;
    }

    ~KcwAutoMutex()
    {
        unlock();
    }

};


#endif /* kcwautomutex_h */
