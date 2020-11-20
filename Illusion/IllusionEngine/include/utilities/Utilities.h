#pragma once

#define USE_STL_VECTOR 1
#define USE_STL_DEQUE 1
#define USE_STL_MAP 1
#define USE_STL_UNORDERED_MAP 1

namespace illusion::util {

#if USE_STL_VECTOR
#include <vector>
	template<typename T>
	using Array = std::vector<T>;

	/**
	 * Fonction permettant de supprimer une valeur de manière efficace pour avoir un tableau sans trous
	 * Le but étant d'échanger la valeur à supprimer avec le dernier element et de "supprimer" le nouveau dernier element
	 * @param v le vecteur à modifier
	 * @param index l'index de la valeur à supprimer
	 */
	template<typename T> void EraseUnordered(Array<T>& v, size_t index) {
		if (v.size() > 1) {
			std::iter_swap(v.begin() + index, v.end() - 1);
			v.pop_back();
		}
		else {
			v.clear();
		}
	}
#endif

#if USE_STL_DEQUE
	#include <deque>
	template<typename T>
	using Deque = std::deque<T>;
#endif

#if USE_STL_MAP
#include <map>
	template < class Key, class T>
	using Map = std::map<Key, T>;

#endif

#if USE_STL_UNORDERED_MAP
#include <unordered_map>
	template < class Key, class T>
	using UnorderedMap = std::unordered_map<Key, T>;
#endif

} //namespace illusion::util