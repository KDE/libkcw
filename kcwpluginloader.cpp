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
#include "kcwpluginloader.h"
#include "kcwdebug.h"

KcwPluginLoader::KcwPluginLoader()
 : m_isLoaded(false)
 , m_instance(NULL) {
}

KcwPluginLoader::instanceFunc KcwPluginLoader::getInstanceFunc() {
	return (KcwPluginLoader::instanceFunc)GetProcAddress((HINSTANCE)m_instance, "getInstance");
}

void KcwPluginLoader::setFileName(std::wstring name) {
	m_fileName = name;
}

bool KcwPluginLoader::setSearchDirectory(std::wstring path) {
	return (bool)SetDllDirectory(path.c_str());
}

bool KcwPluginLoader::isLoaded() const {
	return m_isLoaded;
}

bool KcwPluginLoader::load() {
	if(m_isLoaded) return true;

	if(m_fileName.length() == 0) return false;

	m_instance = (void*)LoadLibrary(m_fileName.c_str());
	if(m_instance == NULL) return false;

	return true;
}

bool KcwPluginLoader::unload() {
	if(!m_isLoaded) return false;
	if(FreeLibrary((HINSTANCE)m_instance)) {
		m_isLoaded = false;
		return true;
	} else {
		return false;
	}
}
