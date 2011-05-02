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
