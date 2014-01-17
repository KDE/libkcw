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
#ifndef kcwdebug_h
#define kcwdebug_h

#include <windows.h>

#include <sstream>
#include <string>
#include <locale>

class KcwDebug;

KcwDebug& endl(KcwDebug& os);

/**
* @author Patrick Spendrin
* @date 2011
* @brief C++ Stream based debug output.
* @details KcwDebug can be used to output most values understood by std::ostream (except wide characters!)
* similar to the following code snippet:
* @code
* KcwDebug() << "the value for x is" << x << endl;
* @endcode
* @note endl is different from std::endl which cannot be used here.
*/

class KcwDebug {
    public:
        /**
        * The default constructor. If the KcwDebug is enabled, output goes to the Windows debug console.
        * You can see output from within debugview (@see http://technet.microsoft.com/en-us/sysinternals/bb896647)
        */
        KcwDebug();

        /**
        * Constructs an object which redirects the output to a string in addition to the debug console.
        * If output is disabled using setEnabled(), the string will still be changed.
        */
        KcwDebug(std::string* result);

        /**
        * the destructor does all the work, to flush the output, the object must be destroyed.
        */
        ~KcwDebug();

        /**
        * template operator method that outputs everything that fits.
        * @return a reference to the object for which this method is called.
        */
        template<typename T> KcwDebug& operator<<(T i) {
            spaceIt();
            m_ss << i;
            return maybeSpaceReference();
        }

        /**
        * typedef for manipulation functions giving special output similar to endl.
        */
        typedef KcwDebug& (ManipFunc)(KcwDebug &);

        /**
        * Output results of manipulation functions.
        * @return a reference to the KcwDebug object.
        */
        KcwDebug& operator<<(ManipFunc manipFunc);

        /**
        * enable or disable debug output according to @p enable.
        * This is a global property, so you only need to set it once in your program.
        */
        static void setEnabled(bool enable);

        /**
        * @return whether debug output is enabled or disabled.
        */
        static bool enabled();

    protected:
        /**
        * output a space character, given that the previous character wasn't an endl
        * to make this work, instead of returning the reference of your KcwDebug object directly
        * you should return maybeSpaceReference() in your overloaded function instead.
        */
        void spaceIt();
        /**
        * @see spaceIt()
        */
        KcwDebug& maybeSpaceReference();

    private:
        static std::locale  s_loc;
        static bool         s_enabled;
        bool                m_maybeSpace;
        std::stringstream   m_ss;
        std::string*        m_stringptr;
        friend KcwDebug& endl(KcwDebug& os);
};

template<> KcwDebug& KcwDebug::operator<<(std::wstring i);
template<> KcwDebug& KcwDebug::operator<<(const wchar_t* i);

#endif /* kcwdebug_h */