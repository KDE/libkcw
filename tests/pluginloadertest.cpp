#include "kcwpluginloader.h"
#include "kcwdebug.h"
#include "testplugin.h"

#define KcwTestAssert(argument, output) if(!(argument)) { OutputDebugString(output); exit(1); }


int main(int argc, char** argv) {
	KcwPluginLoader loader;
	KcwTestAssert(loader.isLoaded() == false, L"Plugin isn't in the correct state")
	loader.setFileName(L"testplugin");
	TestPlugin* plugin = loader.instance<TestPlugin>();
	KcwTestAssert(plugin != 0, L"couldn't get a valid plugin instance")
	KcwTestAssert(plugin->getInt() == 5, L"plugin class doesn't work as expected")
	return 0;
}