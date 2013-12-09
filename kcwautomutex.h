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
