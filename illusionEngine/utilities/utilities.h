#pragma once

#define USE_STL_VECTOR 1
#define USE_STL_DEQUE 1
#define USE_STL_MAP 1
#define USE_STL_UNORDERED_MAP 1

#if USE_STL_VECTOR
#include <vector>
namespace illusion::utl {
template<typename T>
using vector = std::vector<T>;

template<typename T>
void erase_unordered(vector<T>& v, size_t index){
    if(v.size() > 1){
        std::iter_swap(v.begin() + index, v.end() - 1);
        v.pop_back();
    }else{
        v.clear();
    }
}

}
#endif

#if USE_STL_DEQUE
#include <deque>
namespace illusion::utl {
template<typename T>
using deque = std::deque<T>;
}
#endif

#if USE_STL_MAP
#include <map>
namespace illusion::utl {
template < class Key,
           class T>
using map = std::map<Key, T>;
}
#endif

#if USE_STL_UNORDERED_MAP
#include <unordered_map>
namespace illusion::utl {
template < class Key,
           class T>
using unordered_map = std::unordered_map<Key, T>;
}
#endif

namespace illusion::utl {
    //TODO [Romain Saclier] : implement own containers to manage memory
}
