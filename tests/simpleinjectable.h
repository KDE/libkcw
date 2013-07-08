#ifndef simpleinjectable
#define simpleinjectable

#ifdef simpleinjectable_EXPORTS
#define si_EXPORT __declspec(dllexport)
#else
#define si_EXPORT __declspec(dllimport)
#endif

si_EXPORT int startConsole();
#endif /* simpleinjectable */