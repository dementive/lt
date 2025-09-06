#pragma once

#include "lt/defs/crash.hpp"

#include <initializer_list>
#include <stddef.h>

namespace lt {

#ifndef LT_MAP_SIZE_TYPE
#define LT_MAP_SIZE_TYPE unsigned char
#endif

/*
Fixed Map

the same as lt::fixed_set except:

- Elements are a pair
- Has a find() function to find a value from a key. Unlike std::map this returns a KV* instead of an iterator.
- Has a insert() function to add a new key and value to the map.
- The operator[] works different than a std::map. It can only be used to get elements, do not use it to insert elements.
*/
template <typename Key, typename Value, size_t N> struct fixed_map {
private:
	struct KV {
		Key first;
		Value second;
	};
	using Type = KV[N];
	LT_MAP_SIZE_TYPE current_size = 0;
	constexpr void _insert(const Key &p_key, const Value &p_val) {
		if (current_size >= N)
			lt_crash("index out of bounds");

		if (!has(p_key))
			data[current_size++] = KV(p_key, p_val);
	}

public:
	Type data{};

	constexpr size_t size() const { return current_size; }
	constexpr const Value &operator[](const Key &p_key) const {
		KV *value = find(p_key);
		if (value == nullptr)
			lt_crash("index out of bounds");
		return value->second;
	}
	constexpr Value &operator[](const Key &p_key) {
		KV *value = find(p_key);
		if (value == nullptr)
			lt_crash("index out of bounds");
		return value->second;
	}

	constexpr const Type &operator[](size_t p_index) const { return data[p_index]; }
	constexpr Type &operator[](size_t p_index) { return data[p_index]; }

	constexpr const Key &key_at_index(size_t p_key_index) const { return data[p_key_index].first; }
	constexpr const Value &value_at_index(size_t p_key_index) const { return data[p_key_index].second; }

	constexpr void insert(const Key &p_key, const Value &p_val) { _insert(p_key, p_val); }
	constexpr void insert(const KV &p_val) { _insert(p_val.first, p_val.second); }

	constexpr void erase(const Key &p_val) {
		int to_remove = 0;
		for (int i = 0; i < current_size; ++i)
			if (data[i].first == p_val)
				to_remove = i;

		if (to_remove == current_size - 1) {
			data[to_remove].~KV();
			current_size--;
			return;
		}

		data[to_remove] = data[current_size - 1];
		current_size--;
	}

	constexpr bool has(const Key &p_val) {
		for (int i = 0; i < current_size; ++i)
			if (data[i].first == p_val)
				return true;

		return false;
	}

	constexpr KV *find(const Key &p_val) {
		for (int i = 0; i < current_size; ++i)
			if (data[i].first == p_val)
				return &data[i];

		return nullptr;
	}

	constexpr fixed_map() = default;
	constexpr fixed_map(std::initializer_list<KV> p_init) {
		for (const KV &E : p_init)
			if (!has(E.first))
				data[current_size++] = E;
	};

	constexpr const KV *begin() const { return data; }
	constexpr const KV *end() const { return data + current_size; }
	constexpr KV *begin() { return data; }
	constexpr KV *end() { return data + current_size; }
};

#undef LT_MAP_SIZE_TYPE

} // namespace lt
