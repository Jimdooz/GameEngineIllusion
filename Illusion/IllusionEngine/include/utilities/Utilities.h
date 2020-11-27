#pragma once

#define USE_STL_VECTOR 0
#define USE_STL_DEQUE 1
#define USE_STL_MAP 1
#define USE_STL_UNORDERED_MAP 1

#define USE_CHUNCK_ARRAY 1


/**
 * STL SYSTEM
 */

#if USE_STL_VECTOR
#include <vector>
namespace illusion::util {
	template<typename T>
	using Array = std::vector<T>;
}
#endif

#if USE_STL_DEQUE
	#include <deque>
namespace illusion::util {
	template<typename T>
	using Deque = std::deque<T>;
}
#endif

#if USE_STL_MAP
#include <map>
namespace illusion::util {
	template < class Key, class T>
	using Map = std::map<Key, T>;
}
#endif

#if USE_STL_UNORDERED_MAP
#include <unordered_map>
namespace illusion::util {
	template < class Key, class T>
	using UnorderedMap = std::unordered_map<Key, T>;
}
#endif

/**
 * CUSTOM SYSTEM
 */

#if USE_CHUNCK_ARRAY

#include "ChunckArray.h"

namespace illusion::util {
	template<typename T>
	using Array = ChunckArray<T>;
}

#endif

/**
 * Fonctions permettant de manipuler de mani�re plus pr�cises les diff�rentes structures
 */

namespace illusion::util {
	/**
	 * Fonction permettant de supprimer une valeur de mani�re efficace pour avoir un tableau sans trous
	 * Le but �tant d'�changer la valeur � supprimer avec le dernier element et de "supprimer" le nouveau dernier element
	 * @param v le vecteur � modifier
	 * @param index l'index de la valeur � supprimer
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
}