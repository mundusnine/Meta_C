#define MetaC_Doc(...)

#define MetaC_UI

#define MetaC_Printable
#define MetaC_PrintList(...)
#define KINC_FUNC
#define MetaC_Range(...)

#ifdef _WIN32
#if defined(KINC_DYNAMIC)
#define METAC_FUNC __declspec(dllimport)
#elif defined(KINC_DYNAMIC_COMPILE)
#define METAC_FUNC __declspec(dllexport)
#else
#define METAC_FUNC
#endif
#else
#define METAC_FUNC
#endif