#pragma once
 

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <ios>
#include <iostream>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <string>

#if __cplusplus > 201703L
#define  ___HAS_CHAR8T  1
#else
#define  ___HAS_CHAR8T  0
#endif
 
namespace fstd {
 
namespace detail {
namespace impl {
 
template<class CharT, class Traits, class SizeT>
inline SizeT oss_put(std::basic_ostream<CharT, Traits>& os,
    const CharT* data, SizeT size)
{
    return static_cast<SizeT>(os.rdbuf()->sputn(data, size));
}
 
template<class CharT, class Traits, class SizeT>
inline bool oss_fill(std::basic_ostream<CharT, Traits>& os, SizeT size)
{
    CharT c = os.fill();
    CharT fill[] = { c, c, c, c, c, c, c, c };
    enum {
        chunk = sizeof fill / sizeof(CharT)
    };
    for (; size > chunk; size -= chunk) {
        if (oss_put(os, fill, chunk) != chunk) {
            return false;
        }
    }
    return oss_put(os, fill, size) == size;
}
 
template<class CharT, class Traits, class SizeT>
class oss_guard
{
public:
    explicit oss_guard(std::basic_ostream<CharT, Traits>& os) noexcept
        : m_os(&os) { }
 
    ~oss_guard() noexcept(false)
    {
        if (m_os) {
            m_os->setstate(std::basic_ostream<CharT, Traits>::badbit);
        }
    }
 
    void release() noexcept
    {
        m_os = 0;
    }
private:
    oss_guard(const oss_guard&) = default;
    oss_guard& operator=(const oss_guard&) = default;
    std::basic_ostream<CharT, Traits>* m_os;
};
 
} // namespace impl
 
template<class CharT, class Traits, class SizeT>
inline std::basic_ostream<CharT, Traits>& ostream_string(std::basic_ostream<CharT, Traits>& os,
    const CharT* data, SizeT size)
{
    using stream = std::basic_ostream<CharT, Traits>;
    impl::oss_guard<CharT, Traits, SizeT> guard(os);
    typename stream::sentry entry(os);
    if (entry) {
        SizeT width = static_cast<SizeT>(os.width());
        if (width <= size) {
            if (impl::oss_put(os, data, size) != size) {
                return os;
            }
        }
        else if ((os.flags() & stream::adjustfield) == stream::left) {
            if (impl::oss_put(os, data, size) != size ||
                !impl::oss_fill(os, width - size)) {
                return os;
            }
        }
        else if (!impl::oss_fill(os, width - size) ||
            impl::oss_put(os, data, size) != size) {
            return os;
        }
        os.width(0);
    }
    guard.release();
    return os;
}
 
} // namespace detail
 
/**
    \class basic_string_view
    \brief The template class basic_string_view describes an object that can refer to a constant
    contiguous sequence of char-like objects with the first element of the sequence at position zero.
*/
template<typename CharT, class Traits = std::char_traits<CharT>>
class basic_string_view
{
public:
    using traits_type = Traits;
    using value_type = CharT;
    using pointer = CharT * ;
    using const_pointer = const CharT*;
    using reference = CharT & ;
    using const_reference = const CharT&;
    using const_iterator = const_pointer;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    static constexpr size_type npos = size_type(-1);
 
    /**
         Default constructor. Constructs an empty basic_string_view.
         After construction, data() is equal to nullptr, and size() is equal to 0.
    */
    constexpr basic_string_view() noexcept
        : m_ptr(nullptr), m_len(0) {}
 
    /**
        Copy constructor. Constructs a view of the same content as other.
        After construction, data() is equal to other.data(), and size() is equal to other.size().
    */
    constexpr basic_string_view(const basic_string_view& other) noexcept = default;
 
    /**
        Constructs a view of the null-terminated character string pointed to by \a str,
        not including the terminating null character.
        The length of the view is determined as if by Traits::length(str).
        The behavior is undefined if [str, str+Traits::length(str)) is not a valid range.
        After construction, data() is equal to \a str, and size() is equal to Traits::length(str).
    */
    constexpr basic_string_view(const CharT* str)
        : m_ptr(str), m_len(Traits::length(str)) {}
 
    /**
        Constructs a view of the first count characters of the character array starting with the
        element pointed by \a str. \a str can contain null characters.
        The behavior is undefined if [str, str+len) is not a valid range (even though the constructor
        may not access any of the elements of this range). After construction, data() is equal to \a str,
        and size() is equal to \a len.
    */
    constexpr basic_string_view(const CharT* str, size_type len)
        : m_ptr(str), m_len(len) {}
 
    /**
        Replaces the view with that of \a view.
    */
    constexpr basic_string_view& operator=(const basic_string_view& view) noexcept = default;
 
    /**
        Returns an iterator to the first character of the view.
        \sa end(), cbegin(), rbegin()
    */
    constexpr const_iterator begin() const noexcept { return m_ptr; }
    /**
        Returns an iterator to the first character of the view.
        \sa cend(), begin(), crbegin()
    */
    constexpr const_iterator cbegin() const noexcept { return m_ptr; }
    /**
        Returns an iterator to the character following the last character of the view.
        This character acts as a placeholder, attempting to access it results in undefined behavior.
        \sa begin(), cend(), rend()
    */
    constexpr const_iterator end() const noexcept { return m_ptr + m_len; }
    /**
        Returns an iterator to the character following the last character of the view.
        This character acts as a placeholder, attempting to access it results in undefined behavior.
        \sa cbegin(), end(), crend()
    */
    constexpr const_iterator cend() const noexcept { return m_ptr + m_len; }
    /**
        Returns a reverse iterator to the first character of the reversed view.
        It corresponds to the last character of the non-reversed view.
        \sa rend(), begin(), crbegin(),
    */
    constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    /**
        Returns a reverse iterator to the first character of the reversed view.
        It corresponds to the last character of the non-reversed view.
        \sa crend(), rbegin(), cbegin(),
    */
    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    /**
        Returns a reverse iterator to the character following the last character of the reversed view.
        It corresponds to the character preceding the first character of the non-reversed view.
        This character acts as a placeholder, attempting to access it results in undefined behavior.
        \sa rbegin(), crend(), end()
    */
    constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    /**
        Returns a reverse iterator to the character following the last character of the reversed view.
        It corresponds to the character preceding the first character of the non-reversed view.
        This character acts as a placeholder, attempting to access it results in undefined behavior.
        \sa crbegin(), rend(), cend(),
    */
    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
 
    /**
        Returns the number of CharT elements in the view, i.e. std::distance(begin(), end()).
        \sa length(), max_size(), empty()
    */
    constexpr size_type size() const noexcept { return m_len; }
    /**
        Returns the number of CharT elements in the view, i.e. std::distance(begin(), end()).
        \sa size(), max_size(), empty()
    */
    constexpr size_type length() const noexcept { return m_len; }
    /**
        The largest possible number of char-like objects that can be referred to by a view.
        \sa size(), length(), empty()
    */
    constexpr size_type max_size() const noexcept { return m_len; }
    /**
        Checks if the view has no characters, i.e. whether size() == 0.
        \sa size(), length(), max_size()
    */
    constexpr bool empty() const noexcept { return m_len == 0; }
 
    /**
        Returns a const reference to the character at specified location \a pos.
        No bounds checking is performed: the behavior is undefined if pos >= size().
        \note Unlike std::basic_string::operator[], basic_string_view::operator[](size())
            has undefined behavior instead of returning CharT().
        \sa at()
    */
    constexpr const_reference operator[](size_type pos) const noexcept { return m_ptr[pos]; }
 
    /**
        Returns a reference to the character at specified location \a pos with bounds checking.
        \exception std::out_of_range will be thrown on invalid access.
        \sa operator[]()
    */
    constexpr const_reference at(size_t pos) const
    {
        return pos >= m_len ?
            throw std::out_of_range("out of range") : m_ptr[pos];
    }
 
    /**
        Returns reference to the first character in the view.
        The behavior is undefined if empty() == true.
        \sa back(), empty()
    */
    constexpr const_reference front() const { return m_ptr[0]; }
    /**
        Returns reference to the last character in the view.
        The behavior is undefined if empty() == true.
        \sa front(), empty()
    */
    constexpr const_reference back() const { return m_ptr[m_len - 1]; }
    /**
        Returns a pointer to the underlying character array.
        The pointer is such that the range [data(); data() + size()) is valid and the values
        in it correspond to the values of the view.
        \note Unlike std::basic_string::data() and string literals, data() may return a pointer
                to a buffer that is not null-terminated. Therefore it is typically a mistake to pass data()
                to a routine that takes just a const CharT* and expects a null-terminated string.
        \sa front(), back()
    */
    constexpr const_pointer data() const noexcept { return m_ptr; }
 
    /**
        Moves the start of the view forward by \a n characters.
        The behavior is undefined if n > size().
        \sa remove_suffix()
    */
    constexpr void remove_prefix(size_type n)
    {
        if (n > m_len) {
            n = m_len;
        }
        m_ptr += n;
        m_len -= n;
    }
    /**
        Moves the end of the view back by \a n characters.
        The behavior is undefined if n > size().
        \sa remove_prefix()
    */
    constexpr void remove_suffix(size_type n)
    {
        if (n > m_len) {
            n = m_len;
        }
        m_len -= n;
    }
 
    /**
        Exchanges the view with that of \a other.
    */
    constexpr void swap(basic_string_view& other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
        std::swap(m_len, other.m_len);
    }
 
    /**
        Copies the substring [pos, pos + rcount) to the character array pointed to by \a dest,
        where rcount is the smaller of \a count and size() - pos.
        Equivalent to:
        \code
            Traits::copy(dest, data() + pos, rcount).
        \endcode
        \param dest     pointer to the destination character string.
        \param count    requested substring length.
        \param pos      position of the first character
        \return Number of characters copied.
        \exception std::out_of_range if \a pos > size().
        \note std::basic_string_view::copy() has no constexpr specifier before c++20.
        \sa substr()
    */
    constexpr size_type copy(CharT* dest, size_type count, size_type pos = 0) const
    {
        if (pos > size()) {
            throw std::out_of_range("out of range");
        }
        size_type rlen = std::min(count, m_len - pos);
        traits_type::copy(dest, data() + pos, rlen);
        return rlen;
    }
 
    /**
        Returns a view of the substring [pos, pos + rcount),
        where rcount is the smaller of \a count and size() - pos.
        \sa copy(), find()
    */
    constexpr basic_string_view substr(size_type pos, size_type count = npos) const
    {
        if (pos > size()) {
            throw std::out_of_range("out of range");
        }
        return basic_string_view(data() + pos, std::min(size() - pos, count));
    }
 
    /**
        The length rlen of the sequences to compare is the smaller of size() and v.size().
        The function compares the two views by calling Traits::compare(data(), v.data(), rlen),
        and returns a value according to the following table:
        <table>
            <tr><th rowspan="1" colspan="2">Condition                                                            <th> Result                 <th> Retuen Value
            <tr><td rowspan="1" colspan="2">Traits::compare(data(), v.data(), rlen)  < 0                         <td> this is less than v    <td> <0
            <tr><td rowspan="3" colspan="1">Traits::compare(data(), v.data(), rlen) == 0 <td> size()  < v.size() <td> this is less than v    <td> <0
            <tr>                                                                         <td> size() == v.size() <td> this is equal to v     <td>  0
            <tr>                                                                         <td> size()  > v.size() <td> this is greater than v <td> >0
            <tr><td rowspan="1" colspan="2">Traits::compare(data(), v.data(), rlen)  > 0                         <td> this is greater than v <td> >0
        </table>
        \return negative value if this view is less than the other character sequence,
                zero if the both character sequences are equal, positive value if this view is
                greater than the other character sequence.
        \sa operator==(), operator!=(), operator<(), operator>(), operator<=(), operator>=()
    */
    constexpr int compare(basic_string_view v) const noexcept
    {
        const int cmp = Traits::compare(m_ptr, v.m_ptr, std::min(m_len, v.m_len));
        return cmp != 0 ? cmp : (m_len == v.m_len ? 0 : m_len < v.m_len ? -1 : 1);
    }
 
    /**
        Equivalent to:
        \code
            substr(pos1, count1).compare(v);
        \endcode
        \return negative value if this view is less than the other character sequence,
                zero if the both character sequences are equal, positive value if this view is
                greater than the other character sequence.
        \sa substr(), operator==(), operator!=(), operator<(), operator>(), operator<=(), operator>=()
    */
    constexpr int compare(size_type pos1, size_type count1, basic_string_view v) const noexcept
    {
        return substr(pos1, count1).compare(v);
    }
 
    /**
        Equivalent to:
        \code
            substr(pos1, count1).compare(v.substr(pos2, count2));
        \endcode
        \return negative value if this view is less than the other character sequence,
                zero if the both character sequences are equal, positive value if this view is
                greater than the other character sequence.
        \sa substr(), operator==(), operator!=(), operator<(), operator>(), operator<=(), operator>=()
    */
    constexpr int compare(size_type pos1, size_type count1,
        basic_string_view v, size_type pos2, size_type count2) const
    {
        return substr(pos1, count1).compare(v.substr(pos2, count2));
    }
 
    /**
        Equivalent to:
        \code
            compare(basic_string_view(s)).
        \endcode
        \return negative value if this view is less than the other character sequence,
                zero if the both character sequences are equal, positive value if this view is
                greater than the other character sequence.
        \sa operator==(), operator!=(), operator<(), operator>(), operator<=(), operator>=()
    */
    constexpr int compare(const CharT* s) const
    {
        return compare(basic_string_view(s));
    }
 
    /**
        Equivalent to:
        \code
            substr(pos1, count1).compare(basic_string_view(s));
        \endcode
        \return negative value if this view is less than the other character sequence,
                zero if the both character sequences are equal, positive value if this view is
                greater than the other character sequence.
        \sa substr(), operator==(), operator!=(), operator<(), operator>(), operator<=(), operator>=()
    */
    constexpr int compare(size_type pos1, size_type count1, const CharT* s) const
    {
        return substr(pos1, count1).compare(basic_string_view(s));
    }
 
    /**
        Equivalent to:
        \code
            substr(pos1, count1).compare(basic_string_view(s, count2));
        \endcode
        \return negative value if this view is less than the other character sequence,
                zero if the both character sequences are equal, positive value if this view is
                greater than the other character sequence.
        \sa substr(), operator==(), operator!=(), operator<(), operator>(), operator<=(), operator>=()
    */
    constexpr int compare(size_type pos1, size_type count1,
        const CharT* s, size_type count2) const
    {
        return substr(pos1, count1).compare(basic_string_view(s, count2));
    }
 
    /**
        Checks if the string view begins with the given prefix.
        Effectively returns:
        \code
            size() >= x.size() && compare(0, x.size(), x) == 0;
        \endcode
        \note std::basic_string_view provides this function since from c++20, use it carefully.
        \sa ends_with(), compare()
    */
    constexpr bool starts_with(basic_string_view x) const noexcept
    {
        return size() >= x.size() && compare(0, x.size(), x) == 0;
        return m_len >= x.m_len && Traits::compare(m_ptr, x.m_ptr, x.m_len) == 0;
    }
 
    /**
        Checks if the string view begins with the given prefix.
        Effectively returns:
        \code
            !empty() && Traits::eq(front(), x);
        \endcode
        \note std::basic_string_view provides this function since from c++20, use it carefully.
        \sa ends_with(), compare()
    */
    constexpr bool starts_with(CharT x) const noexcept
    {
        return !empty() && Traits::eq(x, front());
    }
 
    /**
        Checks if the string view begins with the given prefix.
        Effectively returns:
        \code
            starts_with(basic_string_view(x));
        \endcode
        \note std::basic_string_view provides this function since from c++20, use it carefully.
        \sa ends_with(), compare()
    */
    constexpr bool starts_with(const CharT* x) const noexcept
    {
        return starts_with(basic_string_view(x));
    }
 
    /**
        Checks if the string view ends with the given suffix.
        Effectively returns:
        \code
            size() >= x.size() && compare(size() - x.size(), npos, x) == 0;
        \endcode
        \note std::basic_string_view provides this function since from c++20, use it carefully.
        \sa starts_with(), compare()
    */
    constexpr bool ends_with(basic_string_view x) const noexcept
    {
        return m_len >= x.m_len &&
            Traits::compare(m_ptr + m_len - x.m_len, x.m_ptr, x.m_len) == 0;
    }
 
    /**
        Checks if the string view ends with the given suffix.
        Effectively returns:
        \code
            !empty() && Traits::eq(back(), x);
        \endcode
        \note std::basic_string_view provides this function since from c++20, use it carefully.
        \sa starts_with(), compare()
    */
    constexpr bool ends_with(CharT x) const noexcept
    {
        return !empty() && Traits::eq(x, back());
    }
 
    /**
        Checks if the string view ends with the given suffix.
        Effectively returns:
        \code
            ends_with(basic_string_view(x)).
        \endcode
        \note std::basic_string_view provides this function since from c++20, use it carefully.
        \sa starts_with(), compare()
    */
    constexpr bool ends_with(const CharT* x) const noexcept
    {
        return ends_with(basic_string_view(x));
    }
 
    /**
        Finds the first substring equal to the given character sequence.
        Finds the first occurence of \a v in this view, starting at position \a pos.
        \return Position of the first character of the found substring, or npos if no such substring is found.
        \sa rfind(), find_first_of(), find_last_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type find(basic_string_view v, size_type pos = 0) const noexcept
    {
        if (pos > size() || v.empty() || v.size() > size() - pos) {
            return npos;
        }
        const CharT* cur = m_ptr + pos;
        const CharT* last = cend() - v.size() + 1;
        for (; cur != last; ++cur) {
            cur = Traits::find(cur, last - cur, v[0]);
            if (!cur) {
                return npos;
            }
            if (Traits::compare(cur, v.cbegin(), v.size()) == 0) {
                return cur - m_ptr;
            }
        }
        return npos;
    }
    /**
        Finds the first substring equal to the given character sequence.
        Equivalent to:
        \code
            find(basic_string_view(std::addressof(ch), 1), pos);
        \endcode
        \return Position of the first character of the found substring, or npos if no such substring is found.
        \sa rfind(), find_first_of(), find_last_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type find(CharT ch, size_type pos = 0) const noexcept
    {
        if (pos <= size()) {
            const CharT* ret_ptr = Traits::find(m_ptr + pos, m_len - pos, ch);
            if (ret_ptr) {
                return ret_ptr - m_ptr;
            }
        }
        return npos;
    }
    /**
        Finds the first substring equal to the given character sequence.
        Equivalent to:
        \code
            find(basic_string_view(s, count), pos).
        \endcode
        \return Position of the first character of the found substring, or npos if no such substring is found.
        \sa rfind(), find_first_of(), find_last_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type find(const CharT* s, size_type pos, size_type count) const noexcept
    {
        return find(basic_string_view(s, count), pos);
    }
    /**
        Finds the first substring equal to the given character sequence.
        Equivalent to:
        \code
            find(basic_string_view(s), pos);
        \endcode
        \return Position of the first character of the found substring, or npos if no such substring is found.
        \sa rfind(), find_first_of(), find_last_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type find(const CharT* s, size_type pos = 0) const noexcept
    {
        return find(basic_string_view(s), pos);
    }
 
    /**
        Finds the last substring equal to the given character sequence.
        Finds the last occurence of \a v in this view, starting at position \a pos.
        \return Position of the first character of the found substring or npos if no such substring is found.
        \sa find(), find_first_of(), find_last_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type rfind(basic_string_view v, size_type pos = npos) const noexcept
    {
        if (m_len < v.m_len) {
            return npos;
        }
        if (pos > m_len - v.m_len) {
            pos = m_len - v.m_len;
        }
        if (v.m_len == 0u) {     // an empty string is always found
            return pos;
        }
        for (const CharT* cur = m_ptr + pos; ; --cur) {
            if (Traits::compare(cur, v.m_ptr, v.m_len) == 0) {
                return cur - m_ptr;
            }
            if (cur == m_ptr) {
                return npos;
            }
        }
    }
    /**
        Finds the last substring equal to the given character sequence.
        Equivalent to:
        \code
            rfind(basic_string_view(std::addressof(c), 1), pos);
        \endcode
        \return Position of the first character of the found substring or npos if no such substring is found.
        \sa find(), find_first_of(), find_last_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type rfind(CharT c, size_type pos = npos) const noexcept
    {
        return rfind(basic_string_view(&c, 1), pos);
    }
    /**
        Finds the last substring equal to the given character sequence.
        Equivalent to:
        \code
            rfind(basic_string_view(s, count), pos);
        \endcode
        \return Position of the first character of the found substring or npos if no such substring is found.
        \sa find(), find_first_of(), find_last_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type rfind(const CharT* s, size_type pos, size_type count) const noexcept
    {
        return rfind(basic_string_view(s, count), pos);
    }
    /**
        Finds the last substring equal to the given character sequence.
        Equivalent to:
        \code
            rfind(basic_string_view(s), pos);
        \endcode
        \return Position of the first character of the found substring or npos if no such substring is found.
        \sa find(), find_first_of(), find_last_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type rfind(const CharT* s, size_type pos = npos) const noexcept
    {
        return rfind(basic_string_view(s), pos);
    }
 
    /**
        Finds the first character equal to any of the characters in the given character sequence.
        Finds the first occurence of any of the characters of \a v in this view, starting at position \a pos.
        \return Position of the first occurrence of any character of the substring, or npos if no such character is found.
        \sa find(), rfind(), find_last_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type find_first_of(basic_string_view v, size_type pos = 0) const noexcept
    {
        if (pos >= m_len || v.m_len == 0) {
            return npos;
        }
        const_iterator iter = std::find_first_of(
            this->cbegin() + pos, this->cend(), v.cbegin(), v.cend(), Traits::eq);
        return iter == this->cend() ? npos : std::distance(this->cbegin(), iter);
    }
    /**
        Finds the first character equal to any of the characters in the given character sequence.
        Equivalent to:
        \code
            find_first_of(basic_string_view(std::addressof(c), 1), pos);
        \endcode
        \return Position of the first occurrence of any character of the substring, or npos if no such character is found.
        \sa find(), rfind(), find_last_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type find_first_of(CharT c, size_type pos = 0) const noexcept
    {
        return find(c, pos);
    }
    /**
        Finds the first character equal to any of the characters in the given character sequence.
        Equivalent to:
        \code
            find_first_of(basic_string_view(s, count), pos);
        \endcode
        \return Position of the first occurrence of any character of the substring, or npos if no such character is found.
        \sa find(), rfind(), find_last_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type find_first_of(const CharT* s, size_type pos, size_type count) const noexcept
    {
        return find_first_of(basic_string_view(s, count), pos);
    }
    /**
        Finds the first character equal to any of the characters in the given character sequence.
        Equivalent to:
        \code
            find_first_of(basic_string_view(s), pos);
        \endcode
        \return Position of the first occurrence of any character of the substring, or npos if no such character is found.
        \sa find(), rfind(), find_last_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type find_first_of(const CharT* s, size_type pos = 0) const noexcept
    {
        return find_first_of(basic_string_view(s), pos);
    }
 
    /**
        Finds the last character equal to one of characters in the given character sequence.
        Exact search algorithm is not specified. The search considers only the interval [0; pos].
        If the character is not present in the interval, npos will be returned.
        Finds the last occurence of any of the characters of v in this view, ending at position pos.
        \return Position of the last occurrence of any character of the substring, or npos if no such character is found.
        \sa find(), rfind(), find_first_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type find_last_of(basic_string_view v, size_type pos = npos) const noexcept
    {
        if (v.m_len == 0u) {
            return npos;
        }
        if (pos >= m_len) {
            pos = 0;
        }
        else {
            pos = m_len - (pos + 1);
        }
        const_reverse_iterator iter = std::find_first_of
        (this->crbegin() + pos, this->crend(), v.cbegin(), v.cend(), Traits::eq);
        return iter == this->crend() ? npos : reverse_distance(this->crbegin(), iter);
    }
    /**
        Finds the last character equal to one of characters in the given character sequence.
        Exact search algorithm is not specified. The search considers only the interval [0; pos].
        If the character is not present in the interval, npos will be returned.
        Equivalent to:
        \code
            find_last_of(basic_string_view(std::addressof(c), 1), pos);
        \endcode
        \return Position of the last occurrence of any character of the substring, or npos if no such character is found.
        \sa find(), rfind(), find_first_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type find_last_of(CharT c, size_type pos = npos) const noexcept
    {
        return find_last_of(basic_string_view(&c, 1), pos);
    }
    /**
        Finds the last character equal to one of characters in the given character sequence.
        Exact search algorithm is not specified. The search considers only the interval [0; pos].
        If the character is not present in the interval, npos will be returned.
        Equivalent to:
        \code
            find_last_of(basic_string_view(s, count), pos);
        \endcode
        \return Position of the last occurrence of any character of the substring, or npos if no such character is found.
        \sa find(), rfind(), find_first_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type find_last_of(const CharT* s, size_type pos, size_type count) const noexcept
    {
        return find_last_of(basic_string_view(s, count), pos);
    }
    /**
        Finds the last character equal to one of characters in the given character sequence.
        Exact search algorithm is not specified. The search considers only the interval [0; pos].
        If the character is not present in the interval, npos will be returned.
        Equivalent to:
        \code
            find_last_of(basic_string_view(s), pos);
        \endcode
        \return Position of the last occurrence of any character of the substring, or npos if no such character is found.
        \sa find(), rfind(), find_first_of(), find_first_not_of(), find_last_not_of()
    */
    constexpr size_type find_last_of(const CharT* s, size_type pos = npos) const noexcept
    {
        return find_last_of(basic_string_view(s), pos);
    }
 
    /**
        Finds the first character not equal to any of the characters in the given character sequence.
        Finds the first character not equal to any of the characters of \a v in this view, starting at position \a pos.
        \return Position of the first character not equal to any of the characters in the given string,
                or npos if no such character is found.
        \sa find(), rfind(), find_first_of(), find_last_of(), find_last_not_of()
    */
    constexpr size_type find_first_not_of(basic_string_view v, size_type pos = 0) const noexcept
    {
        if (pos >= m_len) {
            return npos;
        }
        if (v.m_len == 0) {
            return pos;
        }
        const_iterator iter = find_not_of(this->cbegin() + pos, this->cend(), v);
        return iter == this->cend() ? npos : std::distance(this->cbegin(), iter);
    }
    /**
        Finds the first character not equal to any of the characters in the given character sequence.
        Equivalent to:
        \code
            find_first_not_of(basic_string_view(std::addressof(c), 1), pos);
        \endcode
        \return Position of the first character not equal to any of the characters in the given string,
                or npos if no such character is found.
        \sa find(), rfind(), find_first_of(), find_last_of(), find_last_not_of()
    */
    constexpr size_type find_first_not_of(CharT c, size_type pos = 0) const noexcept
    {
        return find_first_not_of(basic_string_view(&c, 1), pos);
    }
    /**
        Finds the first character not equal to any of the characters in the given character sequence.
        Equivalent to:
        \code
            find_first_not_of(basic_string_view(s, count), pos);
        \endcode
        \return Position of the first character not equal to any of the characters in the given string,
                or npos if no such character is found.
        \sa find(), rfind(), find_first_of(), find_last_of(), find_last_not_of()
    */
    constexpr size_type find_first_not_of(const CharT* s, size_type pos, size_type count) const noexcept
    {
        return find_first_not_of(basic_string_view(s, count), pos);
    }
    /**
        Finds the first character not equal to any of the characters in the given character sequence.
        Equivalent to:
        \code
            find_first_not_of(basic_string_view(s), pos);
        \endcode
        \return Position of the first character not equal to any of the characters in the given string,
                or npos if no such character is found.
        \sa find(), rfind(), find_first_of(), find_last_of(), find_last_not_of()
    */
    constexpr size_type find_first_not_of(const CharT* s, size_type pos = 0) const noexcept
    {
        return find_first_not_of(basic_string_view(s), pos);
    }
 
    /**
        Finds the last character not equal to any of the characters in the given character sequence.
        Finds the last character not equal to any of the characters of \a v in this view, starting at position \a pos.
        \return Position of the last character not equal to any of the characters in the given string,
                or npos if no such character is found.
        \sa find(), rfind(), find_first_of(), find_last_of(), find_first_not_of()
    */
    constexpr size_type find_last_not_of(basic_string_view v, size_type pos = npos) const noexcept
    {
        if (pos >= m_len) {
            pos = m_len - 1;
        }
        if (v.m_len == 0u) {
            return pos;
        }
        pos = m_len - (pos + 1);
        const_reverse_iterator iter = find_not_of(this->crbegin() + pos, this->crend(), v);
        return iter == this->crend() ? npos : reverse_distance(this->crbegin(), iter);
    }
    /**
        Finds the last character not equal to any of the characters in the given character sequence.
        Equivalent to:
        \code
            find_last_not_of(basic_string_view(std::addressof(c), 1), pos);
        \endcode
        \return Position of the last character not equal to any of the characters in the given string,
                or npos if no such character is found.
        \sa find(), rfind(), find_first_of(), find_last_of(), find_first_not_of()
    */
    constexpr size_type find_last_not_of(CharT c, size_type pos = npos) const noexcept
    {
        return find_last_not_of(basic_string_view(&c, 1), pos);
    }
    /**
        Finds the last character not equal to any of the characters in the given character sequence.
        Equivalent to:
        \code
            find_last_not_of(basic_string_view(s, count), pos);
        \endcode
        \return Position of the last character not equal to any of the characters in the given string,
                or npos if no such character is found.
        \sa find(), rfind(), find_first_of(), find_last_of(), find_first_not_of()
    */
    constexpr size_type find_last_not_of(const CharT* s, size_type pos, size_type count) const noexcept
    {
        return find_last_not_of(basic_string_view(s, count), pos);
    }
    /**
        Finds the last character not equal to any of the characters in the given character sequence.
        Equivalent to:
        \code
            find_last_not_of(basic_string_view(s), pos);
        \endcode
        \return Position of the last character not equal to any of the characters in the given string,
                or npos if no such character is found.
        \sa find(), rfind(), find_first_of(), find_last_of(), find_first_not_of()
    */
    constexpr size_type find_last_not_of(const CharT* s, size_type pos = npos) const noexcept
    {
        return find_last_not_of(basic_string_view(s), pos);
    }
private:
    template <typename r_iter>
    size_type reverse_distance(r_iter first, r_iter last) const noexcept
    {
        // Portability note here: std::distance is not NOEXCEPT, but calling it with a string_view::reverse_iterator will not throw.
        return m_len - 1 - std::distance(first, last);
    }
 
    template <typename Iterator>
    Iterator find_not_of(Iterator first, Iterator last, basic_string_view v) const noexcept
    {
        for (; first != last; ++first) {
            if (0 == Traits::find(v.ptr_, v.m_len, *first)) {
                return first;
            }
        }
        return last;
    }
 
private:
    const CharT* m_ptr;
    std::size_t m_len;
}; // class basic_string_view
 
/**
    Compares two views.
    \li All comparisons are done via the compare() member function (which itself is defined in terms of Traits::compare()):
        Two views are equal if both the size of lhs and rhs are equal and each character in \a x has an equivalent character
        in \a y at the same position.
    \li The ordering comparisons are done lexicographically -- the comparison is performed by a function equivalent to
        std::lexicographical_compare.
    \return \c true if the corresponding comparison holds, \c false otherwise.
*/
template<typename CharT, typename Traits>
inline constexpr bool operator==(basic_string_view<CharT, Traits> x, basic_string_view<CharT, Traits> y) noexcept
{
    if (x.size() != y.size()) {
        return false;
    }
    return x.compare(y) == 0;
}
 
/**
    Compares two views.
    \li All comparisons are done via the compare() member function (which itself is defined in terms of Traits::compare()):
        Two views are equal if both the size of lhs and rhs are equal and each character in \a x has an equivalent character
        in \a y at the same position.
    \li The ordering comparisons are done lexicographically -- the comparison is performed by a function equivalent to
        std::lexicographical_compare.
    \return \c true if the corresponding comparison holds, \c false otherwise.
*/
template<typename CharT, typename Traits>
inline constexpr bool operator!=(basic_string_view<CharT, Traits> x, basic_string_view<CharT, Traits> y) noexcept
{
    if (x.size() != y.size()) {
        return true;
    }
    return x.compare(y) != 0;
}
 
/**
    Compares two views.
    \li All comparisons are done via the compare() member function (which itself is defined in terms of Traits::compare()):
        Two views are equal if both the size of lhs and rhs are equal and each character in \a x has an equivalent character
        in \a y at the same position.
    \li The ordering comparisons are done lexicographically -- the comparison is performed by a function equivalent to
        std::lexicographical_compare.
    \return \c true if the corresponding comparison holds, \c false otherwise.
*/
template<typename CharT, typename Traits>
inline constexpr bool operator<(basic_string_view<CharT, Traits> x, basic_string_view<CharT, Traits> y) noexcept
{
    return x.compare(y) < 0;
}
 
/**
    Compares two views.
    \li All comparisons are done via the compare() member function (which itself is defined in terms of Traits::compare()):
        Two views are equal if both the size of lhs and rhs are equal and each character in \a x has an equivalent character
        in \a y at the same position.
    \li The ordering comparisons are done lexicographically -- the comparison is performed by a function equivalent to
        std::lexicographical_compare.
    \return \c true if the corresponding comparison holds, \c false otherwise.
*/
template<typename CharT, typename Traits>
inline constexpr bool operator>(basic_string_view<CharT, Traits> x, basic_string_view<CharT, Traits> y) noexcept
{
    return x.compare(y) > 0;
}
 
/**
    Compares two views.
    \li All comparisons are done via the compare() member function (which itself is defined in terms of Traits::compare()):
        Two views are equal if both the size of lhs and rhs are equal and each character in \a x has an equivalent character
        in \a y at the same position.
    \li The ordering comparisons are done lexicographically -- the comparison is performed by a function equivalent to
        std::lexicographical_compare.
    \return \c true if the corresponding comparison holds, \c false otherwise.
*/
template<typename CharT, typename Traits>
inline constexpr bool operator<=(basic_string_view<CharT, Traits> x, basic_string_view<CharT, Traits> y) noexcept
{
    return x.compare(y) <= 0;
}
 
/**
    Compares two views.
    \li All comparisons are done via the compare() member function (which itself is defined in terms of Traits::compare()):
        Two views are equal if both the size of lhs and rhs are equal and each character in \a x has an equivalent character
        in \a y at the same position.
    \li The ordering comparisons are done lexicographically -- the comparison is performed by a function equivalent to
        std::lexicographical_compare.
    \return \c true if the corresponding comparison holds, \c false otherwise.
*/
template<typename CharT, typename Traits>
inline constexpr bool operator>=(basic_string_view<CharT, Traits> x, basic_string_view<CharT, Traits> y) noexcept
{
    return x.compare(y) >= 0;
}
 
/**
    Equivalent to:
    \code
        x == basic_string_view<CharT, Traits>(y);
    \endcode
*/
template<typename CharT, typename Traits, typename Allocator>
inline constexpr bool operator==(basic_string_view<CharT, Traits> x,
    const std::basic_string<CharT, Traits, Allocator>& y) noexcept
{
    return x == basic_string_view<CharT, Traits>(y);
}
 
/**
    Equivalent to:
    \code
        basic_string_view<CharT, Traits>(x) == y;
    \endcode
*/
template<typename CharT, typename Traits, typename Allocator>
inline constexpr bool operator==(const std::basic_string<CharT, Traits, Allocator>& x,
    basic_string_view<CharT, Traits> y) noexcept
{
    return basic_string_view<CharT, Traits>(x) == y;
}
 
/**
    Equivalent to:
    \code
        x == basic_string_view<CharT, Traits>(y);
    \endcode
*/
template<typename CharT, typename Traits>
inline constexpr bool operator==(basic_string_view<CharT, Traits> x, const CharT* y) noexcept
{
    return x == basic_string_view<CharT, Traits>(y);
}
 
/**
    Equivalent to:
    \code
        basic_string_view<CharT, Traits>(x) == y;
    \endcode
*/
template<typename CharT, typename Traits>
inline constexpr bool operator==(const CharT* x, basic_string_view<CharT, Traits> y) noexcept
{
    return basic_string_view<CharT, Traits>(x) == y;
}
 
/**
    Equivalent to:
    \code
        x != basic_string_view<CharT, Traits>(y);
    \endcode
*/
template<typename CharT, typename Traits, typename Allocator>
inline constexpr bool operator!=(basic_string_view<CharT, Traits> x,
    const std::basic_string<CharT, Traits, Allocator>& y) noexcept
{
    return x != basic_string_view<CharT, Traits>(y);
}
 
/**
    Equivalent to:
    \code
        basic_string_view<CharT, Traits>(x) != y;
    \endcode
*/
template<typename CharT, typename Traits, typename Allocator>
inline constexpr bool operator!=(const std::basic_string<CharT, Traits, Allocator>& x,
    basic_string_view<CharT, Traits> y) noexcept
{
    return basic_string_view<CharT, Traits>(x) != y;
}
 
/**
    Equivalent to:
    \code
        x != basic_string_view<CharT, Traits>(y);
    \endcode
*/
template<typename CharT, typename Traits>
inline constexpr bool operator!=(basic_string_view<CharT, Traits> x, const CharT* y) noexcept
{
    return x != basic_string_view<CharT, Traits>(y);
}
 
/**
    Equivalent to:
    \code
        basic_string_view<CharT, Traits>(x) != y;
    \endcode
*/
template<typename CharT, typename Traits>
inline constexpr bool operator!=(const CharT* x, basic_string_view<CharT, Traits> y) noexcept
{
    return basic_string_view<CharT, Traits>(x) != y;
}
 
/**
    Equivalent to:
    \code
        x < basic_string_view<CharT, Traits>(y);
    \endcode
*/
template<typename CharT, typename Traits, typename Allocator>
inline constexpr bool operator<(basic_string_view<CharT, Traits> x,
    const std::basic_string<CharT, Traits, Allocator>& y) noexcept
{
    return x < basic_string_view<CharT, Traits>(y);
}
 
/**
    Equivalent to:
    \code
        basic_string_view<CharT, Traits>(x) < y;
    \endcode
*/
template<typename CharT, typename Traits, typename Allocator>
inline constexpr bool operator<(const std::basic_string<CharT, Traits, Allocator>& x,
    basic_string_view<CharT, Traits> y) noexcept
{
    return basic_string_view<CharT, Traits>(x) < y;
}
 
/**
    Equivalent to:
    \code
        x < basic_string_view<CharT, Traits>(y);
    \endcode
*/
template<typename CharT, typename Traits>
inline constexpr bool operator<(basic_string_view<CharT, Traits> x, const CharT* y) noexcept
{
    return x < basic_string_view<CharT, Traits>(y);
}
 
/**
    Equivalent to:
    \code
        basic_string_view<CharT, Traits>(x) < y;
    \endcode
*/
template<typename CharT, typename Traits>
inline constexpr bool operator<(const CharT* x, basic_string_view<CharT, Traits> y) noexcept
{
    return basic_string_view<CharT, Traits>(x) < y;
}
 
/**
    Equivalent to:
    \code
        x > basic_string_view<CharT, Traits>(y);
    \endcode
*/
template<typename CharT, typename Traits, typename Allocator>
inline constexpr bool operator>(basic_string_view<CharT, Traits> x,
    const std::basic_string<CharT, Traits, Allocator>& y) noexcept
{
    return x > basic_string_view<CharT, Traits>(y);
}
 
/**
    Equivalent to:
    \code
        basic_string_view<CharT, Traits>(x) > y;
    \endcode
*/
template<typename CharT, typename Traits, typename Allocator>
inline constexpr bool operator>(const std::basic_string<CharT, Traits, Allocator>& x,
    basic_string_view<CharT, Traits> y) noexcept
{
    return basic_string_view<CharT, Traits>(x) > y;
}
 
/**
    Equivalent to:
    \code
        x > basic_string_view<CharT, Traits>(y);
    \endcode
*/
template<typename CharT, typename Traits>
inline constexpr bool operator>(basic_string_view<CharT, Traits> x,
    const CharT* y) noexcept
{
    return x > basic_string_view<CharT, Traits>(y);
}
 
/**
    Equivalent to:
    \code
        basic_string_view<CharT, Traits>(x) > y;
    \endcode
*/
template<typename CharT, typename Traits>
inline constexpr bool operator>(const CharT* x, basic_string_view<CharT, Traits> y) noexcept
{
    return basic_string_view<CharT, Traits>(x) > y;
}
 
/**
    Equivalent to:
    \code
        x <= basic_string_view<CharT, Traits>(y);
    \endcode
*/
template<typename CharT, typename Traits, typename Allocator>
inline constexpr bool operator<=(basic_string_view<CharT, Traits> x,
    const std::basic_string<CharT, Traits, Allocator>& y) noexcept
{
    return x <= basic_string_view<CharT, Traits>(y);
}
 
/**
    Equivalent to:
    \code
        basic_string_view<CharT, Traits>(x) <= y;
    \endcode
*/
template<typename CharT, typename Traits, typename Allocator>
inline constexpr bool operator<=(const std::basic_string<CharT, Traits, Allocator>& x,
    basic_string_view<CharT, Traits> y) noexcept
{
    return basic_string_view<CharT, Traits>(x) <= y;
}
 
/**
    Equivalent to:
    \code
        x <= basic_string_view<CharT, Traits>(y);
    \endcode
*/
template<typename CharT, typename Traits>
inline constexpr bool operator<=(basic_string_view<CharT, Traits> x, const CharT* y) noexcept
{
    return x <= basic_string_view<CharT, Traits>(y);
}
 
/**
    Equivalent to:
    \code
        basic_string_view<CharT, Traits>(x) <= y;
    \endcode
*/
template<typename CharT, typename Traits>
inline constexpr bool operator<=(const CharT* x, basic_string_view<CharT, Traits> y) noexcept
{
    return basic_string_view<CharT, Traits>(x) <= y;
}
 
/**
    Equivalent to:
    \code
        x >= basic_string_view<CharT, Traits>(y);
    \endcode
*/
template<typename CharT, typename Traits, typename Allocator>
inline constexpr bool operator>=(basic_string_view<CharT, Traits> x,
    const std::basic_string<CharT, Traits, Allocator>& y) noexcept
{
    return x >= basic_string_view<CharT, Traits>(y);
}
 
/**
    Equivalent to:
    \code
        basic_string_view<CharT, Traits>(x) >= y;
    \endcode
*/
template<typename CharT, typename Traits, typename Allocator>
inline constexpr bool operator>=(const std::basic_string<CharT, Traits, Allocator>& x,
    basic_string_view<CharT, Traits> y) noexcept
{
    return basic_string_view<CharT, Traits>(x) >= y;
}
 
/**
    Equivalent to:
    \code
        x >= basic_string_view<CharT, Traits>(y);
    \endcode
*/
template<typename CharT, typename Traits>
inline constexpr bool operator>=(basic_string_view<CharT, Traits> x, const CharT* y) noexcept
{
    return x >= basic_string_view<CharT, Traits>(y);
}
 
/**
    Equivalent to:
    \code
        basic_string_view<CharT, Traits>(x) >= y;
    \endcode
*/
template<typename CharT, typename Traits>
inline constexpr bool operator>=(const CharT* x, basic_string_view<CharT, Traits> y) noexcept
{
    return basic_string_view<CharT, Traits>(x) >= y;
}
 
/**
    Behaves as a FormattedOutputFunction.
    After constructing and checking the sentry object, determines the output format padding as follows:
    \li If v.size() is not less than os.width(), uses the range [v.begin(), v.end()) as-is.
    \li Otherwise, if (os.flags() & ios_base::adjustfield) == ios_base::left, places os.width()-v.size()
        copies of the os.fill() character after the character sequence.
    \li Otherwise, places os.width()-v.size() copies of the os.fill() character before the character sequence.
    Then stores each character from the resulting sequence (the contents of v plus padding) to the output
    stream os as if by calling os.rdbuf()->sputn(seq, n), where n=std::max(os.width(), str.size()).
    Finally, calls os.width(0) to cancel the effects of std::setw, if any.
    \exception May throw \c std::ios_base::failure if an exception is thrown during output.
*/
template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
    const basic_string_view<CharT, Traits> str)
{
    return detail::ostream_string(os, str.data(), str.size());
}
 
using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;
#if ___HAS_CHAR8T
using u8string_view = basic_string_view<char8_t>;
#endif
 
inline namespace string_view_literals {
 
/**
    Forms a string view of a character literal.
    \return The string_view literal.
*/
//[[nodiscard]] constexpr string_view operator "" sv(const char* str, std::size_t len) noexcept
//{
//    return (string_view(str, len));
//}
 
/**
    Forms a string view of a character literal.
    \return The string_view literal.
*/
//[[nodiscard]] constexpr wstring_view operator "" sv(const wchar_t* str, std::size_t len) noexcept
//{
//    return (wstring_view(str, len));
//}
 
/**
    Forms a string view of a character literal.
    \return The string_view literal.
*/
//[[nodiscard]] constexpr u16string_view operator "" sv(const char16_t* str, std::size_t len) noexcept
//{
//    return (u16string_view(str, len));
//}
 
/**
    Forms a string view of a character literal.
    \return The string_view literal.
*/
//[[nodiscard]] constexpr u32string_view operator "" sv(const char32_t* str, std::size_t len) noexcept
//{
//    return (u32string_view(str, len));
//}
 
#if ___HAS_CHAR8T
 
/**
    Forms a string view of a character literal.
    \return The string_view literal.
*/
//[[nodiscard]] constexpr u8string_view operator "" sv(const char8_t* str, std::size_t len) noexcept
//{
//    return (u8string_view(str, len));
//}
 
#endif // ___HAS_CHAR8T
 
} // namespace string_view_literals
 
} // namespace fstd