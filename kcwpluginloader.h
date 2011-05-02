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
