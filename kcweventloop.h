#ifndef kcweventloop_h
#define kcweventloop_h

#include <windows.h>
#include <vector>

typedef void (*eventCallback)(void *obj);

template <typename T> class KcwSharedMemory;

/**
* @author Patrick Spendrin
* @date 2011-2013
*
* @brief KcwEventLoop is the event loop base class which handles the event distribution.
*
* If you want to override the default application wide event handle for this loop, 
* give it as parameter or set it before calling exec().
*/
class KcwEventLoop {
    public:
        KcwEventLoop(HANDLE eventHandle = NULL);
        virtual ~KcwEventLoop();

        /**
        * This function can be used to add handles to wait for and a callback to be called 
        * when this specific handle got signaled. Since this is a primitive implementation,
        * calling addCallback after exec gives undefined behavior.
        * if @p singleCall is true, the callback will be removed from the eventloop after it has fired
        */
        void addCallback(HANDLE hndl, eventCallback cllbck = NULL, void *callbackObject = NULL, bool singleCall = false);
        void addCallback(HANDLE hndl, HANDLE notification, bool singleCall = false);

        void removeCallback(HANDLE hndl, eventCallback cllbck = NULL);

        /**
        * call this function to run the event loop and to get notified.
        */
        int exec();

        /**
        * quit the event loop.
        */
        virtual void quit();

        /**
        * change the wait interval for each handle; the default value is 10 milliseconds.
        */
        void setRefreshInterval(int msecs);

        /**
        * return the currently set wait interval for each handle in milliseconds.
        */
        int refreshInterval() const;

        /**
        * sets the default event used to signal that the event loop should be quit.
        */
        virtual void setExitEvent(HANDLE event);

        /**
        * returns the default event that is used to signal that the event loop should be quit.
        */
        virtual HANDLE exitEvent();

        /**
         * returns the event loop id
         */
        int eventLoopId() const;

    private:
        bool callForObject(unsigned objNum);

        void removeCallback(unsigned n);
        // returns the new number of callbacks available
        unsigned cleanupCallbacks();

        static void handleCallback(HANDLE obj);
        std::vector<eventCallback> m_callbacks;
        std::vector<void*> m_objects;
        std::vector<HANDLE> m_handles;
        std::vector<int> m_removeCallbacks;
        std::vector<bool> m_singleCall;
    protected:
        int m_refreshInterval;
        const int m_eventLoopId;
        HANDLE m_eventHandle;
        CRITICAL_SECTION m_criticalSection;
        static int getUniqueCounter();
        static KcwSharedMemory<int> s_globalEventLoopCounter;
};

#define KCW_CALLBACK(class, name) \
void  name();\
static void name##static(void *obj) { reinterpret_cast<class*>(obj)->name(); }
#define CB(name) &name##static
#endif /* kcweventloop_h */
