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
#ifndef kcwthread_h
#define kcwthread_h

#include <windows.h>

#include "kcweventloop.h"
#include "kcwthreadrep.h"

/**
* @author Patrick Spendrin
* @date 2011-2013
*
* @brief This class is supposed to wrap thread handling.
*
* To use this class, inherit KcwThread and overload run() in your class.
* If you want to start your thread, just call start() (you don't need to
* overload that function). The default implementation just starts an eventloop.
*/

class KcwThread : public KcwEventLoop, public KcwThreadRep {
    public:
        /**
        * Constructor for a thread object. If you want to override the default
        * event used for signaling the end of the thread, add it as the parameter.
        * @param exitEventHandle    the event used for signaling the end of the
        *                           handle; if exitEventHandle is NULL, a new Event will
        *                           be created.
        * @warning signaling the exitEvent is currently not threadsave. You should
        * @warning not change the exitEvent status after you started the thread.
        */
        KcwThread(HANDLE exitEventHandle = NULL);
        ~KcwThread();

        /**
        * this function starts the thread. The function run() will be called in
        * the thread.
        */
        virtual void start();

        /**
         * pauses this thread.
         */
        virtual void pause();

        /**
        * this function is called when the thread is running. There is an eventLoop
        * running per default. You can override this behaviour by overloading run().
        */
        virtual DWORD run();

    private:
        static DWORD WINAPI monitorThreadStatic(LPVOID lpParameter);
        DWORD monitorThread();

        HANDLE m_thread;
};
#endif /* kcwthread_h */
