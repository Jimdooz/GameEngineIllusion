#pragma once

#include "common/CommonHeaders.h"

namespace illusion::util {
	template<typename T>
	class ChunckArray {
	public:
		ChunckArray(u32 size = 0, u32 reserved = -1, u32 chunckSize = -1) {
			if (reserved == -1) reserved = size;

			m_chunckSize = chunckSize == -1 ? ChunckArray::chunckSize : chunckSize;
			bool toAdd = reserved == 0 || reserved % m_chunckSize > 0 ? true : false;

			reserved -= reserved % m_chunckSize;

			m_chunckNumber = (reserved / m_chunckSize) + (toAdd ? 1 : 0);

			m_size = size;
			m_realSize = m_chunckNumber * m_chunckSize;
			m_content = new T[m_realSize];
		}
		~ChunckArray() {
			delete m_content;
		}
		T& operator[](size_t idx) {
			return m_content[idx];
		}

		void push_back(T value) {
			if (m_size > m_realSize - 1) Upgrade();
			m_content[m_size] = value;
			m_size++;
		}

		void pop_back() {
			m_size = --m_size <= 0 ? 0 : m_size;
		}

		void resize(u32 size) {
			if (size > m_realSize) {
				Upgrade(size - m_realSize);
			}
			m_size = m_realSize;
		}

		T& back() {
			if (m_size <= 0) throw "Invalid Index";
			return m_content[m_size - 1];
		}

		u32 size() {
			return m_size;
		}

		static const u32 chunckSize = 512;

	protected:
		void Upgrade(u32 size = 1) {
			m_chunckNumber += size;
			m_chunckSize *= 2;
			T* content = new T[m_chunckNumber * m_chunckSize];
			T* result = content;
			T* start = m_content; T* end = m_content + m_realSize;
			while (start != end) {
				*result++ = *start++;
			}
			m_realSize = m_chunckNumber * m_chunckSize;
			delete m_content;
			m_content = content;
		}

		u32 m_chunckSize;
		u32 m_chunckNumber;
		u32 m_realSize;

		u32 m_size;
		T* m_content;
	};
}