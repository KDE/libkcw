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
#ifndef kcwpluginloader_h
#define kcwpluginloader_h

/**
* @author Patrick Spendrin
* @date 2011
* @brief This class is used to abstract a plugin system
*/

#include <windows.h>
#include <string>

class KcwPluginLoader {
    public:
        /**
        * Returns an empty plugin loader object.
        */
        KcwPluginLoader();

		/**
		* @return an instance of the plugin or 0 in case of error.
		* if needed this function loads the plugin.
		*/
		template<typename T>
		T* instance() {
			if(!isLoaded()) {
				if(load()) m_isLoaded = true;
				else return 0;
			}
			instanceFunc func = getInstanceFunc();
			return static_cast<T*>(func());
		}

		/**
		* set the file name of the plugin (without extension).
		*/
		void setFileName(std::wstring name);

		/**
		* set another directory where to search for the plugin.
		* @return true if the operation succeeded
		*/
		bool setSearchDirectory(std::wstring path);

		/**
		* @return true if plugin has been loaded
		*/
		bool isLoaded() const;
		
		/**
		* @def KCW_ADD_PLUGIN(PluginClass)
		* @PluginClass 
		*/
#define KCW_ADD_PLUGIN(PluginClass) \
		extern "C" __declspec(dllexport) void* getInstance() {\
			return (void*)new PluginClass;\
		}
	private:
		/**
		* Load the plugin. Calling load multiple times, just triggers a load once.
		* @return true in case the plugin could be loaded
		*/
		bool load();
		
		/**
		* Unload the plugin. Calling unload multiple times, just triggers an unload once.
		* @return true in case the plugin could be unloaded.
		*/
		bool unload();

		typedef void*(__cdecl *instanceFunc)(void);

		instanceFunc getInstanceFunc();
		
		void*			m_instance;
		bool			m_isLoaded;

		std::wstring 	m_fileName;
};
#endif /* kcwpluginloader_h */
