#pragma once
#include "CommonHeaders.h"

// LOGING interne au moteur
#ifdef ILSENGINE_INTERNAL_DEBUG
#define ILSENGINE_INTERNAL_INFO(...) illusion::log(__FILE__,"(",__LINE__,") ","[INTERNAL] - INFO: ",__VA_ARGS__,"\n");
#define ILSENGINE_INTERNAL_WARNING(...) illusion::log(__FILE__,"(",__LINE__,") ","[INTERNAL] - WARNING: ",__VA_ARGS__,"\n");
#define ILSENGINE_INTERNAL_ERROR(...) illusion::log(__FILE__,"(",__LINE__,") ","[INTERNAL] - ERROR: ",__VA_ARGS__,"\n");
#else
#define ILSENGINE_INTERNAL_INFO(...)
#define ILSENGINE_INTERNAL_WARNING(...)
#define ILSENGINE_INTERNAL_ERROR(...)
#endif

// LOGING externe au moteur
#ifdef ILSENGINE_DEBUG
#define ILSENGINE_INFO(...) illusion::log(__FILE__,"(",__LINE__,") ","INFO: ",__VA_ARGS__,"\n");
#define ILSENGINE_WARNING(...) illusion::log(__FILE__,"(",__LINE__,") ","WARNING: ",__VA_ARGS__,"\n");
#define ILSENGINE_ERROR(...) illusion::log(__FILE__,"(",__LINE__,") ","ERROR: ",__VA_ARGS__,"\n");
#else
#define ILSENGINE_INFO(...)
#define ILSENGINE_WARNING(...)
#define ILSENGINE_ERROR(...)
#endif

#if (defined(ILSENGINE_DEBUG) || defined(ILSENGINE_INTERNAL_DEBUG))
namespace illusion {
	template<typename Arg1, typename ...Args>
	void log(Arg1&& arg1, Args&& ...args)
	{
		std::cout << arg1;
		log(args...);
	}
	template <typename Arg1>
	void log(Arg1&& arg1)
	{
		std::cout << arg1;
	}
};
#endif