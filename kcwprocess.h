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
#ifndef kcwprocess_h
#define kcwprocess_h

/**
* @author Patrick Spendrin
* @date 2011
* @brief KcwProcess is used for process abstraction.
*
* @details This class is used to abstract a process, either a running one or
* to start a new one.
*/

#include <windows.h>
#include <string>
#include <map>

#include "kcwthreadrep.h"
#include "kcweventloop.h"

class KcwProcess {
    public:
        /**
        * start a new process using the executable @p execPath.
        */
        KcwProcess(std::wstring execPath);

        /**
        * wrap an already running process with @p pid.
        */
        KcwProcess(int pid);

        /**
        * the default constructor: you can set the executable path later.
        */
        KcwProcess();

        typedef enum {  KCW_STDIN_HANDLE,
                        KCW_STDOUT_HANDLE,
                        KCW_STDERR_HANDLE
                     } KCW_STREAM_TYPE;

        /**
        * set a handle to one of input, output or error stream of the process.
        */
        void setStdHandle(HANDLE hdl, KCW_STREAM_TYPE type);

        /**
         * returns the standard handle of the type @p type.
         */
        HANDLE stdHandle(KCW_STREAM_TYPE type) const;

        /**
        * when starting a process, you can start it in a paused state and resume() it later.
        */
        void setIsStartedAsPaused(bool isPaused);

        /**
        * set some additional startup flags in @p stFlags .
        */
        void setStartupFlags(int stFlags);

        /**
        * start the process from the specified executable.
        * @return true in case the process could be started, false otherwise.
        */
        bool start();

        /**
        * In case the process got started, resume the main thread.
        * @return true in case the process had been suspended and could be resumed, 
        * false otherwise.
        */
        bool resume();

        /**
        * close the process again.
        */
        KCW_CALLBACK(KcwProcess, quit)

        /**
        * @return the flags used for startup. The default value is
        * CREATE_NEW_CONSOLE | CREATE_SUSPENDED.
        * @see http://msdn.microsoft.com/en-us/library/ms684863%28v=vs.85%29.aspx
        */
        int startupFlags() const;

        /**
        * @return the handle of the process that is represented.
        */
        HANDLE process() const;

        /**
        * @return the handle of the main thread of the process.
        */
        HANDLE thread() const;

        /**
        * @return a thread representation for the main thread.
        */
        KcwThreadRep threadRep() const;

        /**
        * @return whether the process is running.
        */
        bool isRunning() const;

        /**
        * @return the process id of the process.
        */
        int pid() const;

        /**
        * @return the command to be executed.
        */
        std::wstring cmd() const;

        /**
         * sets the command to execute.
         */
        void setCmd(const std::wstring& _cmd);

        void setStartupAsHidden(bool x = true);
        bool startupAsHidden() const;

        /**
         * @return the exitcode of the process after it finished
         */
        int exitCode() const;

        /**
         * set the process's initial working directory
         */
        void setInitialWorkingDirectory(const std::wstring& iwd);

        /**
         * @return the initial working directory for this process
         */
        std::wstring initialWorkingDirectory() const;



        struct KcwProcessEnvironment : public std::map<std::wstring, std::wstring> {
            static KcwProcessEnvironment getCurrentEnvironment();
            /*
            static KcwProcessEnvironment getDefaultEnvironment();
             */
        };
        /**
         * Set the environment that is used when the process starts
         */
        void setStartupEnvironment(KcwProcessEnvironment env);
        KcwProcessEnvironment getStartupEnvironment() const;
        void addEnvironmentEntry(std::wstring var, std::wstring value);

    private:
        KcwThreadRep        m_threadRep;
        HANDLE              m_stdHandles[3];
        std::wstring        m_cmd;
        std::wstring        m_initialWorkingDirectory;
        KcwProcessEnvironment   m_environment;
        int                 m_startupFlags;
        bool                m_isRunning;
        bool                m_isStartedAsPaused;
        bool                m_isStartedAsHidden;
};


#endif /* kcwprocess_h */
