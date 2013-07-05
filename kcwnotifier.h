#ifndef kcwnotifier
#define kcwnotifier

#include <iostream>
#include <windows.h>

/**
* @author Patrick Spendrin
* @date 2013
* @brief a cross application notification class
* @details KcwNotifier is useful to wrap windows events and make them accessible from a different process
*/

class KcwNotifier {
    public:
        KcwNotifier();
        KcwNotifier(const std::wstring& strName);

        ~KcwNotifier();

        /**
         * opens an existing event or creates a new one.
         */
        int open();

        /**
         * opens an existing event or creates a new one.
         */
        int open(const std::wstring& strName);

        /**
         * closes an event. In case this event is not opened anywhere else, it will be deleted by the system.
         */
        void close();

        /**
         * returns true if the event is opened
         */
        bool opened() const;

        /**
         * notify this event. As soon as this has been registered by an eventloop, this event is automatically reset.
         */
        void notify();

        /**
         * returns the handle that needs to be added to the eventloops.
         */
        HANDLE handle();
    private:
        HANDLE m_event;
};

#endif /* kcwnotifier */