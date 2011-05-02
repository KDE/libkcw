#include "kcwpluginloader.h"
#include "testplugin.h"

TestPlugin::TestPlugin() : m_int(5) {
}

int TestPlugin::getInt() const {
	return m_int;
}

KCW_ADD_PLUGIN(TestPlugin)