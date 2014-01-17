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
         * (re-)sets the name of this event
         */
        void setNotifierName(const std::wstring& strName);

        /**
         * returns the name of this event
         */
        std::wstring notifierName() const;

        /**
         * notify this event. As soon as this has been registered by an eventloop, this event is automatically reset.
         */
        void notify();

        /**
         * returns the handle that needs to be added to the eventloops.
         */
        HANDLE handle();

        /**
         * implicitly converts a notifier into a HANDLE
         */
        operator HANDLE();
    private:
        std::wstring m_name;
        HANDLE m_event;
};

#endif /* kcwnotifier */