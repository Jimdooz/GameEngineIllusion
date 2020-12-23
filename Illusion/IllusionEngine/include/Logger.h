#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

// LOGING interne au moteur
#ifdef ILSENGINE_INTERNAL_DEBUG
#define INTERNAL_INFO(...) illusion::log(__FILENAME__,"(",__LINE__,") ","[INTERNAL] INFO: ",__VA_ARGS__,"\n");
#define INTERNAL_WARN(...) illusion::log(__FILENAME__,"(",__LINE__,") ","[INTERNAL] WARNING: ",__VA_ARGS__,"\n");
#define INTERNAL_ERR(...) illusion::log(__FILENAME__,"(",__LINE__,") ","[INTERNAL] ERROR: ",__VA_ARGS__,"\n");
#else
#define INTERNAL_INFO(...)
#define INTERNAL_WARN(...)
#define INTERNAL_ERR(...)
#endif

// LOGING externe au moteur
#ifdef ILSENGINE_DEBUG
#define INFO(...) illusion::log(__FILENAME__,"(",__LINE__,") ","[!] INFO: ",__VA_ARGS__,"\n");
#define WARN(...) illusion::log(__FILENAME__,"(",__LINE__,") ","/!\\ WARNING: ",__VA_ARGS__,"\n");
#define ERR(...) illusion::log(__FILENAME__,"(",__LINE__,") ","/x\\ ERROR: ",__VA_ARGS__,"\n");
#else
#define INFO(...)
#define WARN(...)
#define ERR(...)
#endif

#if (defined(ILSENGINE_DEBUG) || defined(ILSENGINE_INTERNAL_DEBUG))
namespace illusion {
	template <typename Arg1>
	void log(Arg1&& _arg1)
	{
		std::cout << _arg1;
	}
	template<typename Arg1,typename ...Args>
	void log(Arg1&& _arg1, Args&& ..._args)
	{
		std::cout << _arg1;
		log(_args...);
	}
};
#endif