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

#include "kcwthreadrep.h"
#include "kcweventloop.h"

class KcwProcess {
    public:
        /**
        * start a new process using the executable @p execPath.
        */
        KcwProcess(std::string execPath);

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
        std::string cmd() const;

        /**
         * sets the command to execute.
         */
        void setCmd(const std::string& _cmd);

        /**
         * @return the exitcode of the process after it finished
         */
        int exitCode() const;

    private:
        KcwThreadRep        m_threadRep;
        HANDLE              m_stdHandles[3];
        std::string         m_cmd;
        int                 m_startupFlags;
        bool                m_isRunning;
        bool                m_isStartedAsPaused;
};


#endif /* kcwprocess_h */
