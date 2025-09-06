#pragma once

namespace lt {

namespace detail {

// Constexpr C string functions
constexpr void strncpy(char *dest, const char *src, int num) {
	int i = 0;
	while (i < num && src[i] != '\0') {
		dest[i] = src[i];
		++i;
	}
	while (i < num) {
		dest[i] = '\0';
		++i;
	}
}

constexpr int strlen(const char *p_str) {
	int i = 0;
	while (p_str[i] != '\0')
		++i;
	return i;
}

constexpr int strcmp(const char *s1, const char *s2) {
	while (*s1 != '\0' && *s1 == *s2) {
		s1++;
		s2++;
	}
	return static_cast<int>(*s1) - static_cast<int>(*s2);
}

} // namespace detail

// Fixed size string
template <int N> struct fixed_string {
private:
	char cstr[N + 1] = {}; // + 1 for null terminator

public:
	constexpr fixed_string() = default;

	constexpr fixed_string(const char *other) {
		if (other != nullptr) {
			lt::detail::strncpy(this->cstr, other, N + 1);
			if (this->cstr[N])
				this->cstr[N] = 0;
		}
	}

	constexpr fixed_string(const fixed_string &other) { lt::detail::strncpy(this->cstr, other.cstr, N + 1); }

	constexpr fixed_string(const char &other) {
		this->cstr[0] = other;
		if (this->cstr[N])
			this->cstr[N] = 0;
	}

	constexpr operator char *() { return cstr; }
	constexpr operator const char *() { return (const char *)cstr; }

	constexpr fixed_string &operator=(const char *other) {
		if (other != nullptr) {
			lt::detail::strncpy(this->cstr, other, N + 1);
			if (this->cstr[N])
				this->cstr[N] = 0;
		}
		return *this;
	}

	constexpr fixed_string &operator=(const fixed_string &other) {
		if not consteval {
			if (this != &other)
				lt::detail::strncpy(this->cstr, other.cstr, N + 1);
		} else {
			lt::detail::strncpy(this->cstr, other.cstr, N + 1);
		}
		return *this;
	}

	template <int M> constexpr fixed_string &operator=(fixed_string<M> &other) {
		lt::detail::strncpy(this->cstr, other.c_str(), N + 1);
		if (this->cstr[N])
			this->cstr[N] = 0;
		return *this;
	}

	constexpr fixed_string &operator=(const char &other) {
		this->cstr[0] = other;
		if (this->cstr[N]) {}
#if N > 0
		this->cstr[1] = 0;
#else
		this->cstr[N] = 0;
#endif
		return *this;
	}

	constexpr bool operator==(const char *other) const { return !lt::detail::strcmp(this->cstr, other); }
	constexpr bool operator==(char *other) const { return !lt::detail::strcmp(this->cstr, other); }
	constexpr bool operator==(const fixed_string &other) const { return !lt::detail::strcmp(this->cstr, other.cstr); }
	constexpr bool operator==(fixed_string &other) const { return !lt::detail::strcmp(this->cstr, other.cstr); }

	constexpr bool operator!=(const char *other) const { return lt::detail::strcmp(this->cstr, other); }
	constexpr bool operator!=(char *other) const { return lt::detail::strcmp(this->cstr, other); }
	constexpr bool operator!=(const fixed_string &other) const { return lt::detail::strcmp(this->cstr, other.cstr); }
	constexpr bool operator!=(fixed_string &other) const { return lt::detail::strcmp(this->cstr, other.cstr); }

	constexpr char &operator[](int i) { return cstr[i]; }

	constexpr const char *c_str() const { return (const char *)cstr; }
	constexpr int length() { return lt::detail::strlen(cstr); }
	constexpr int max_size() { return N; }

	constexpr fixed_string<N> substr(int pos = 0, int len = N + 1) {
		fixed_string<N> r;
		if (pos < lt::detail::strlen(this->cstr))
			lt::detail::strncpy(r.cstr, this->cstr + pos, len);
		return r;
	}

	constexpr bool ends_with(const char *p_str) {
		int lStr = lt::detail::strlen(p_str);
		int lThis = lt::detail::strlen(cstr);
		if (lStr > lThis)
			return false;
		return !lt::detail::strcmp(p_str, cstr + (lThis - lStr));
	}
};

using small_cstr = fixed_string<7>;
using cstr = fixed_string<15>;
using big_cstr = fixed_string<31>;
using huge_cstr = fixed_string<255>;

} // namespace lt
