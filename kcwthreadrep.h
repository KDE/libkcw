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
#ifndef kcwthreadrep
#define kcwthreadrep

#include <windows.h>
#include <vector>

/**
* @author Patrick Spendrin
* @date 2011
* @brief a simple thread representation
* @details KcwThreadRep wraps a thread in any application and gives basic functionality.
*/
class KcwThreadRep {
    public:
        /**
        * create a default thread representation object.
        */
        KcwThreadRep();

        /**
        * create a thread representation for an application local thread with thread handle @p thread.
        * this is equivalent to calling KcwThreadRep ktr(GetCurrentProcess(void), thread); .
        */
        KcwThreadRep(HANDLE thread);

        /**
        * create a thread representation for a thread with thread handle @p thread in the application with
        * process handle @p app.
        */
        KcwThreadRep(HANDLE app, HANDLE thread);

        /**
        * convenience constructor; equivalent to @code KcwThreadRep(KcwThreadRep::getHandleForThreadId(threadid)) @endcode
        */
        KcwThreadRep(int threadId);

        /**
        * convenience constructor; equivalent to 
        * @code KcwThreadRep(KcwThreadRep::getHandleForThreadId(pid), 
        *                    KcwThreadRep::getHandleForThreadId(pid)) @endcode.
        */
        KcwThreadRep(int pid, int threadid);

        /**@{*/
        /**
        * attach to the thread with handle @p thread of the local application.
        */
        void attachThread(HANDLE thread);

        /**
        * attach to the thread with handle @p thread of the application with handle @p app.
        */
        void attachAppThread(HANDLE app, HANDLE thread);

        /**
        * convenience method for @see attachThread(HANDLE thread).
        */
        void attachThreadId(int threadId);
        /**@}*/

        /**
        * @return true in case the thread had been suspended and could be resumed, 
        * false otherwise.
        */
        bool resume();

        /**
        * @return true in case the thread had been running and could be suspended, 
        * false otherwise.
        */
        bool suspend();

        /**
        * @return the handle of the thread
        */
        HANDLE threadHandle() const;

        /**
        * @return the handle to the process this thread lives in
        */
        HANDLE processHandle() const;

        /**
        * @return representations of all threads in the process with the handle @p app
        */
        static std::vector<KcwThreadRep> threads(HANDLE app);

        /**
        * @return representations of all threads in the process with the process id @p pid
        */
        static std::vector<KcwThreadRep> threads(int pid);

        /**
        * @return the process handle for the application with the process id @p pid
        */
        static HANDLE getHandleForPid(int pid);

        /**
        * @return the handle for the thread with thread id @p threadid
        */
        static HANDLE getHandleForThreadId(int threadid);
    private:
        HANDLE m_process;
        HANDLE m_thread;
};

#endif /* kcwthreadrep */
