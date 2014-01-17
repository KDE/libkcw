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
#include <iostream>
#include <string>

#include <windows.h>

#include "kcwdebug.h"

bool KcwDebug::s_enabled = true;
std::locale KcwDebug::s_loc("");

KcwDebug::KcwDebug()
 :  m_maybeSpace( false ),
    m_stringptr( NULL )
{}

KcwDebug::KcwDebug(std::string* ptr)
 :  m_maybeSpace( false ),
    m_stringptr( ptr )
{}

KcwDebug::~KcwDebug() {
    if(s_enabled) {
//        std::cout << m_ss.str();
        OutputDebugStringA(m_ss.str().c_str());
    }
    if(m_stringptr) *m_stringptr = m_ss.str();
}

KcwDebug& KcwDebug::maybeSpaceReference() {
    m_maybeSpace = true;
    return *this;
}

void KcwDebug::spaceIt() {
    if(m_maybeSpace) m_ss << " ";
    m_maybeSpace = false;
}

KcwDebug& KcwDebug::operator<<(ManipFunc manipFunc) {
    return manipFunc(*this);
}

void KcwDebug::setEnabled(bool enable) {
    s_enabled = enable;
}

bool KcwDebug::enabled() {
    return s_enabled;
}

// manipulation function
/**
* writes an end of line character (std::endl) either to the debug console or the string
* used by the KcwDebug object.
*/
KcwDebug& endl(KcwDebug& os) {
    // before endl, we don't need a space
    os.m_maybeSpace = false;
    os.m_ss << std::endl;
    return os;
}

template<> KcwDebug& KcwDebug::operator<<(std::wstring i) {
    spaceIt();
    const wchar_t *wstr = i.c_str();
    char *str = new char[i.length() + 1];
    ZeroMemory(str, (i.length() + 1) * sizeof(char));
    std::use_facet<std::ctype<wchar_t> >(s_loc).narrow(wstr, wstr+wcslen(wstr), '?', str);
    str[wcslen(wstr)] = 0;
    m_ss << str;
    return maybeSpaceReference();
}

template<> KcwDebug& KcwDebug::operator<<(const wchar_t* i) {
    return KcwDebug::operator<<(std::wstring(i));
}
