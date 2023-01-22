#ifdef LIB3DMM_EXPORT
#define LIB3DMM_API __declspec(dllexport)
#else
#define LIB3DMM_API __declspec(dllimport)
#endif

extern "C" LIB3DMM_API void hello(void);
