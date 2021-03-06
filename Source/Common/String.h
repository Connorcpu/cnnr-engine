#pragma once

#include <LuaInterface/IncludeLua.h>
#include <Common/Platform.h>
#include <Common/Filesystem.h>
#include <Common/Hash.h>
#include <string>
#include <gsl.h>
#include <iostream>

class String
{
    enum Tag { Owned, Ref };
    union Data
    {
        inline Data(std::string &&owned);
        inline Data(gsl::cstring_span<> ref);
        inline ~Data() {}

        std::string owned;
        gsl::cstring_span<> ref;
    };

    Tag tag;
    Data data;

public:
    inline ~String();
    inline String();

    inline String(const String &other);
    inline String(String &&other);

    // Refer to a cstr
    inline String(const char *cstr);
    // Copy a reference
    inline String(gsl::cstring_span<> str);
    // Takes ownership
    inline String(std::string &&owned);
    // String literals
    template <size_t len>
    inline String(const char str[len]);

    inline String &operator=(const String &other);
    inline String &operator=(String &&other);

    inline String &operator+=(const String &rhs);
    
    inline void reset();
    static inline String to_owned(String &&str);
    inline String into_owned() &&;
    inline String clone() const;
    inline std::string into_stdstring() &&;
    inline std::string &as_owned();
    inline std::string copy_owned() const;
    inline gsl::cstring_span<> span() const;
    inline fs::path path() const;
    inline String &append(const String &rhs);
    inline String &replace(char from, char to);

    const char *begin() const;
    const char *end() const;

    const char *c_str();

    inline bool empty() const;
    inline size_t length() const;
    inline size_t size() const;

    inline operator gsl::cstring_span<>() const;

    inline void push_lua(lua_State *L) const;
    inline static String from_lua(lua_State *L, int idx);
};

inline String::Data::Data(std::string &&owned)
    : owned(std::move(owned))
{
}

inline String::Data::Data(gsl::cstring_span<> ref)
    : ref(ref)
{
}

inline String operator ""_s(const char *str, size_t len)
{
    return gsl::cstring_span<>{ str, (ptrdiff_t)len };
}

inline String::~String()
{
    if (tag == Owned)
        data.owned.~basic_string();
}

inline String::String()
    : tag(Ref), data(gsl::cstring_span<>{ "" })
{
}

inline String::String(const String &other)
    : tag(Ref), data(other)
{
    if (other.tag == Owned)
    {
        tag = Owned;
        new (&data.owned) std::string(other.data.owned);
    }
}

inline String::String(String &&other)
    : String()
{
    tag = other.tag;
    if (tag == Owned)
        new (&data.owned) std::string(std::move(other.data.owned));
    else
        data.ref = other.data.ref;
}

inline String::String(const char *cstr)
    : String(gsl::cstring_span<>{ cstr, (ptrdiff_t)strlen(cstr) })
{
}

inline String::String(gsl::cstring_span<> str)
    : tag(Ref), data(str)
{
}

inline String::String(std::string &&owned)
    : tag(Owned), data(std::move(owned))
{
}

inline String &String::operator=(const String &other)
{
    if (tag == Owned && other.tag == Owned)
    {
        data.owned = other.data.owned;
    }
    else
    {
        this->String::~String();
        new (this) String(other);
    }
    return *this;
}

inline String &String::operator=(String &&other)
{
    this->String::~String();
    new (this) String(std::move(other));
    return *this;
}

inline String &String::operator+=(const String &rhs)
{
    return append(rhs);
}

inline void String::reset()
{
    this->String::~String();
    new (this) String();
}

inline String String::to_owned(String &&str)
{
    if (str.tag == Owned)
        return std::move(str);

    gsl::cstring_span<> span = str;
    return String(std::string{ span.begin(), span.end() });
}

inline String String::into_owned() &&
{
    return String::to_owned(std::move(*this));
}

inline String String::clone() const
{
    return String(span()).into_owned();
}

inline std::string String::into_stdstring() &&
{
    return std::move(std::move(*this).into_owned().data.owned);
}

inline std::string &String::as_owned()
{
    *this = to_owned(std::move(*this));
    return data.owned;
}

inline std::string String::copy_owned() const
{
    return std::move(String(*this).as_owned());
}

inline gsl::cstring_span<> String::span() const
{
    return *this;
}

inline fs::path String::path() const
{
    return fs::path{ begin(), end() };
}

inline String &String::append(const String &rhs)
{
    as_owned().append(rhs.begin(), rhs.end());
    return *this;
}

inline String &String::replace(char from, char to)
{
    auto &s = as_owned();
    std::transform(s.begin(), s.end(), s.begin(), [from, to](char c) { return c == from ? to : c; });
    return *this;
}

inline const char *String::begin() const
{
    return span().data();
}

inline const char *String::end() const
{
    return span().data() + span().size();
}

inline const char *String::c_str()
{
    return as_owned().c_str();
}

inline bool String::empty() const
{
    return size() == 0;
}

inline size_t String::length() const
{
    gsl::cstring_span<> span = *this;
    return span.size();
}

inline size_t String::size() const
{
    return length();
}

inline String::operator gsl::cstring_span<>() const
{
    switch (tag)
    {
        case Owned:
            return data.owned;
        case Ref:
            return data.ref;
        default:
            unreachable();
    }
}

inline void String::push_lua(lua_State *L) const
{
    lua_pushlstring(L, span().data(), span().size());
}

inline String String::from_lua(lua_State * L, int idx)
{
    if (lua_type(L, idx) != LUA_TSTRING)
        throw std::runtime_error{ "Expected string" };
    size_t len;
    const char *str = lua_tolstring(L, idx, &len);
    return String{ gsl::cstring_span<>{ str, (ptrdiff_t)len } };
}

template<size_t len>
inline String::String(const char str[len])
    : tag(Ref), data(gsl::cstring_span<>{ str })
{
}

inline String operator+(String &&lhs, const String &rhs)
{
    String temp = std::move(lhs);
    temp.as_owned().append(rhs.begin(), rhs.end());
    return std::move(temp);
}

inline String operator+(const String &lhs, const String &rhs)
{
    return String(lhs) + rhs;
}

inline bool operator==(const String &lhs, const String &rhs)
{
    return static_cast<gsl::cstring_span<>>(lhs) == static_cast<gsl::cstring_span<>>(rhs);
}

inline bool operator==(const String &lhs, const char *rhs)
{
    return lhs == String(rhs);
}

inline bool operator==(const char *lhs, const String &rhs)
{
    return String(lhs) == rhs;
}

inline bool operator!=(const String &lhs, const String &rhs)
{
    return static_cast<gsl::cstring_span<>>(lhs) != static_cast<gsl::cstring_span<>>(rhs);
}

inline bool operator<(const String &lhs, const String &rhs)
{
    return static_cast<gsl::cstring_span<>>(lhs) > static_cast<gsl::cstring_span<>>(rhs);
}

inline bool operator>(const String &lhs, const String &rhs)
{
    return static_cast<gsl::cstring_span<>>(lhs) < static_cast<gsl::cstring_span<>>(rhs);
}

inline bool operator<=(const String &lhs, const String &rhs)
{
    return static_cast<gsl::cstring_span<>>(lhs) <= static_cast<gsl::cstring_span<>>(rhs);
}

inline bool operator>=(const String &lhs, const String &rhs)
{
    return static_cast<gsl::cstring_span<>>(lhs) >= static_cast<gsl::cstring_span<>>(rhs);
}

inline std::ostream &operator<<(std::ostream &lhs, const String &rhs)
{
    lhs.write(rhs.span().data(), rhs.span().size());
    return lhs;
}

inline fs::path operator/(fs::path &&lhs, const String &rhs)
{
    fs::path temp = std::move(lhs);
    temp.append(rhs.begin(), rhs.end());
    return std::move(temp);
}

inline fs::path operator/(const fs::path &lhs, const String &rhs)
{
    return fs::path(lhs) / rhs;
}

template <typename H>
inline void hash_apply(const String &str, H &h)
{
    gsl::cstring_span<> span = str;
    hash_apply(span, h);
}

inline String tostring(String &&str)
{
    return std::move(str);
}

inline String tostring(const String &str)
{
    return str;
}

inline String tostring(const char *str)
{
    return String(str);
}

namespace std
{
    inline String tostring(std::string &&str)
    {
        return String(std::move(str));
    }

    inline String tostring(const std::string &str)
    {
        return String(std::string(str));
    }
}

inline String tostring(uint32_t u)
{
    return std::to_string(u);
}

inline String tostring(int32_t i)
{
    return std::to_string(i);
}

inline String tostring(uint64_t u)
{
    return std::to_string(u);
}

inline String tostring(int64_t i)
{
    return std::to_string(i);
}

inline String tostring(float f)
{
    return std::to_string(f);
}

inline String tostring(double d)
{
    return std::to_string(d);
}

template <typename T>
inline String operator+(String &&lhs, T &&rhs)
{
    auto temp = std::move(lhs);
    temp.append(tostring(std::forward<T>(rhs)));
    return std::move(temp);
}

template <typename T>
inline String operator+(const String &lhs, T &&rhs)
{
    String temp = tostring(std::forward<T>(rhs));
    temp.as_owned().insert(0, lhs.span().data(), lhs.span().size());
    return std::move(temp);
}
