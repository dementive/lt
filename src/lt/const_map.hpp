#pragma once

#include <initializer_list>
#include <stddef.h>

#include "lt/defs/crash.hpp"

namespace lt {

/*
Constant map

- Can only be created with an initializer_list.
- Values cannot be modified after creation.
- Stores values in an array.
- Does not do dynamic memory allocation.
*/
template <typename Key, typename Value, size_t Size> struct const_map {
private:
	struct KV {
		Key first;
		Value second;
	};
	using Type = KV[Size];
	using KeyType = Key[Size];
	using ValueType = Value[Size];

	Type data;

public:
	constexpr const Value &at(const Key &key) const {
		for (const KV &pair : data)
			if (pair.first == key)
				return pair.second;

		lt_crash("key not found");
		// throws compiler error (without using exceptions) if key doens't exist, there is probably a better way to do this that will show an actual error message.
		auto var = data[(18446744073709551615UL)];
		return var.second;
	}

	constexpr const Value &operator[](const Key &key) const { return at(key); }
	constexpr const Value &operator[](size_t p_key_index) const { return data[p_key_index].second; }

	constexpr const Key &key_at_index(size_t p_key_index) const { return data[p_key_index].first; }
	constexpr const Value &value_at_index(size_t p_key_index) const { return data[p_key_index].second; }

	constexpr size_t size() const { return Size; }

	constexpr const KeyType &keys() const {
		KeyType keys;
		for (int i = 0; i < Size; ++i)
			keys[i] = data[i].first;
		return keys;
	}

	constexpr const ValueType &values() const {
		ValueType keys;
		for (int i = 0; i < Size; ++i)
			keys[i] = data[i].second;
		return keys;
	}

	const_map() = delete /*("const_map can only be constructed with an initializer_list.")*/;
	constexpr const_map(std::initializer_list<KV> p_init) {
		for (int i = 0; const KV &E : p_init)
			data[i++] = E;
	};

	constexpr KV *begin() { return data; }
	constexpr KV *end() { return data + Size; }

	constexpr const KV *begin() const { return data; }
	constexpr const KV *end() const { return data + Size; }
};

} // namespace lt
