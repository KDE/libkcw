#ifndef testplugin_h
#define testplugin_h

class TestPlugin {
	public:
		TestPlugin();
		virtual int getInt() const;
	private:
		int m_int;
};
#endif /* testplugin_h */
