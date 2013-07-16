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
        void start();

        /**
         * pauses this thread.
         */
        void pause();

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
