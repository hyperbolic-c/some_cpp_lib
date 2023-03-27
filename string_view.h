// #ifndef LIBRARY_STRING_VIEW
// #define LIBRARY_STRING_VIEW
#ifndef STRING_VIEW_H
#define STRING_VIEW_H

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

namespace david
{

  // NOTE: using https://en.cppreference.com/w/cpp/string/basic_string_view as a
  // guide.
  template <class CharT, class Traits = std::char_traits<CharT>>
  class basic_string_view
  {
  public:
    // Types.
    using traits_type = Traits;
    using value_type = CharT;
    using pointer = CharT *;
    using const_pointer = const CharT *;
    using reference = CharT &;
    using const_reference = const CharT &;
    using const_iterator = const CharT *;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    static constexpr size_type npos = size_type(-1);

    // Construction.
    constexpr basic_string_view() noexcept : data_(nullptr), len_(0) {}
    constexpr basic_string_view(const basic_string_view &other) noexcept =
        default;
    constexpr basic_string_view(const_pointer str)
        : data_(str), len_(internal_strlen(str)) {}
    constexpr basic_string_view(const_pointer str, size_type len)
        : data_(str), len_(len) {}
    basic_string_view(const std::string &str)
        : data_(str.data()), len_(str.size()) {}

    // Assignment.
    basic_string_view &operator=(const basic_string_view &) noexcept = default;

    // Iterator support.
    constexpr const_iterator begin() const noexcept { return data_; }
    constexpr const_iterator cbegin() const noexcept { return begin(); }
    constexpr const_iterator end() const noexcept { return data_ + len_; }
    constexpr const_iterator cend() const noexcept { return end(); }
    // TODO: these cannot be marked as constexpr because std::reverse_iterator is
    // not constexpr.
    const_reverse_iterator rbegin() const noexcept
    {
      return const_reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator rend() const noexcept
    {
      return const_reverse_iterator(begin());
    }
    const_reverse_iterator crend() const noexcept { return rend(); }

    // Element access.
    constexpr const_reference operator[](size_type pos) const
    {
      return data_[pos];
    }
    const_reference at(size_type pos) const
    {
      if (pos > len_)
      {
        throw std::out_of_range("Out of range");
      }

      return this->operator[](pos);
    }
    constexpr const_reference front() const { return data_[0]; }
    constexpr const_reference back() const { return data_[len_ - 1]; }
    constexpr const_pointer data() const noexcept { return data_; }

    // Capacity.
    constexpr size_type size() const noexcept { return len_; }
    constexpr size_type length() const noexcept { return size(); }
    constexpr size_type max_size() const noexcept { return kMaxSize; }
    constexpr bool empty() const noexcept { return len_ == 0; }

    // Modifiers.
    void remove_prefix(size_type n)
    {
      data_ = data_ + n;
      len_ -= n;
    }
    void remove_suffix(size_type n) { len_ -= n; }
    void swap(basic_string_view &s) noexcept
    {
      basic_string_view other = *this;
      *this = s;
      s = other;
    }

    // Operations.
    // Copies the substring [pos, pos + rcount) to the character string pointed to
    // by dest, where rcount is the smaller of count and size() - pos.
    size_type copy(pointer dest, size_type count, size_type pos = 0) const
    {
      if (pos > len_)
      {
        throw std::out_of_range("Out of range");
      }

      const size_type rcount = std::min(count, len_ - pos);
      traits_type::copy(dest, data_ + pos, rcount);
      return rcount;
    }
    // Returns a view of the substring [pos, pos + rcount), where rcount is the
    // smaller of count and size() - pos.
    basic_string_view substr(size_type pos = 0, size_type count = npos) const
    {
      if (pos > len_)
      {
        throw std::out_of_range("Out of range");
      }

      const size_type rcount = std::min(count, len_ - pos);
      if (rcount > 0)
      {
        return basic_string_view(data_ + pos, rcount);
      }
      return basic_string_view();
    }
    // Compares two character sequences.
    int compare(basic_string_view s) const noexcept
    {
      const size_t rlen = std::min(len_, s.len_);
      const int comparison = traits_type::compare(data_, s.data_, rlen);
      if (comparison != 0)
        return comparison;
      if (len_ == s.len_)
        return 0;
      return len_ < s.len_ ? -1 : 1;
    }
    // Compare substring(pos1, count1) with s.
    int compare(size_type pos1, size_type count1, basic_string_view s) const
    {
      return substr(pos1, count1).compare(s);
    }
    // Compare substring(pos1, count1) with s.substring(pos2, count2).
    int compare(size_type pos1, size_type count1, basic_string_view s,
                size_type pos2, size_type count2) const
    {
      return substr(pos1, count1).compare(s.substr(pos2, count2));
    }
    int compare(const_pointer s) const { return compare(basic_string_view(s)); }
    int compare(size_type pos1, size_type count1, const_pointer s) const
    {
      return substr(pos1, count1).compare(basic_string_view(s));
    }
    int compare(size_type pos1, size_type count1, const_pointer s,
                size_type count2) const
    {
      return substr(pos1, count1).compare(basic_string_view(s, count2));
    }
    bool starts_with(basic_string_view s) const noexcept
    {
      return len_ >= s.len_ && substr(0, s.len_) == s;
    }
    bool starts_with(value_type c) const noexcept
    {
      return !empty() && traits_type::eq(front(), c);
    }
    bool starts_with(const_pointer s) const
    {
      return starts_with(basic_string_view<CharT, Traits>(s));
    }
    bool ends_with(basic_string_view s) const noexcept
    {
      return len_ >= s.len_ && substr(len_ - s.len_, npos) == s;
    }
    bool ends_with(value_type c) const noexcept
    {
      return !empty() && traits_type::eq(back(), c);
    }
    bool ends_with(const_pointer s) const
    {
      return ends_with(basic_string_view<CharT, Traits>(s));
    }
    size_type find(basic_string_view s, size_type pos = 0) const noexcept
    {
      if (empty() && s.empty() && pos == 0)
      {
        return 0;
      }
      if (pos > len_ || s.len_ > (len_ - pos))
      {
        return npos;
      }
      while (pos + s.len_ <= len_)
      {
        if (traits_type::compare(data_ + pos, s.data_, s.len_) == 0)
        {
          return pos;
        }

        pos++;
      }

      return npos;
    }
    size_type find(value_type c, size_type pos = 0) const noexcept
    {
      return find(basic_string_view(&c, 1), pos);
    }
    size_type find(const_pointer s, size_type pos, size_type n) const
    {
      return find(basic_string_view(s, n), pos);
    }
    size_type find(const_pointer s, size_type pos = 0) const
    {
      return find(basic_string_view(s), pos);
    }
    size_type rfind(basic_string_view s, size_type pos = npos) const noexcept
    {
      if (s.empty())
      {
        return std::min(pos, len_);
      }
      if (s.len_ > len_)
      {
        return npos;
      }
      pos = std::min(pos, len_ - s.len_);
      while (pos != npos)
      {
        if (traits_type::compare(data_ + pos, s.data_, s.len_) == 0)
        {
          return pos;
        }

        pos--;
      }

      return npos;
    }
    size_type rfind(value_type c, size_type pos = npos) const noexcept
    {
      return rfind(basic_string_view(&c, 1), pos);
    }
    size_type rfind(const_pointer s, size_type pos, size_type n) const
    {
      return rfind(basic_string_view(s, n), pos);
    }
    size_type rfind(const_pointer s, size_type pos = npos) const
    {
      return rfind(basic_string_view(s), pos);
    }
    size_type find_first_of(basic_string_view s,
                            size_type pos = 0) const noexcept
    {
      while (pos < len_)
      {
        if (traits_type::find(s.data_, s.len_, data_[pos]) != nullptr)
        {
          return pos;
        }

        pos++;
      }

      return npos;
    }
    size_type find_first_of(value_type c, size_type pos = 0) const noexcept
    {
      return find_first_of(basic_string_view(&c, 1), pos);
    }
    size_type find_first_of(const_pointer s, size_type pos, size_type n) const
    {
      return find_first_of(basic_string_view(s, n), pos);
    }
    size_type find_first_of(const_pointer s, size_type pos = 0) const
    {
      return find_first_of(basic_string_view(s), pos);
    }
    size_type find_last_of(basic_string_view s,
                           size_type pos = npos) const noexcept
    {
      if (empty())
      {
        return npos;
      }

      pos = std::min(pos, len_ - 1);
      while (pos != npos)
      {
        if (traits_type::find(s.data_, s.len_, data_[pos]) != nullptr)
        {
          return pos;
        }

        pos--;
      }

      return npos;
    }
    size_type find_last_of(value_type c, size_type pos = npos) const noexcept
    {
      return find_last_of(basic_string_view(&c, 1), pos);
    }
    size_type find_last_of(const_pointer s, size_type pos, size_type n) const
    {
      return find_last_of(basic_string_view(s, n), pos);
    }
    size_type find_last_of(const_pointer s, size_type pos = npos) const
    {
      return find_last_of(basic_string_view(s), pos);
    }
    size_type find_first_not_of(basic_string_view s,
                                size_type pos = 0) const noexcept
    {
      while (pos < len_)
      {
        if (traits_type::find(s.data_, s.len_, data_[pos]) == nullptr)
        {
          return pos;
        }

        pos++;
      }

      return npos;
    }
    size_type find_first_not_of(value_type c, size_type pos = 0) const noexcept
    {
      return find_first_not_of(basic_string_view(&c, 1), pos);
    }
    size_type find_first_not_of(const_pointer s, size_type pos,
                                size_type n) const
    {
      return find_first_not_of(basic_string_view(s, n), pos);
    }
    size_type find_first_not_of(const_pointer s, size_type pos = 0) const
    {
      return find_first_not_of(basic_string_view(s), pos);
    }
    size_type find_last_not_of(basic_string_view s,
                               size_type pos = npos) const noexcept
    {
      if (empty())
      {
        return npos;
      }

      pos = std::min(pos, len_ - 1);
      while (pos != npos)
      {
        if (traits_type::find(s.data_, s.len_, data_[pos]) == nullptr)
        {
          return pos;
        }

        pos--;
      }

      return npos;
    }
    size_type find_last_not_of(value_type c,
                               size_type pos = npos) const noexcept
    {
      return find_last_not_of(basic_string_view(&c, 1), pos);
    }
    size_type find_last_not_of(const_pointer s, size_type pos,
                               size_type n) const
    {
      return find_last_not_of(basic_string_view(s, n), pos);
    }
    size_type find_last_not_of(const_pointer s, size_type pos = npos) const
    {
      return find_last_not_of(basic_string_view(s), pos);
    }

    // We need these to be friend non-member functions because we want to compare
    // operands with different types (for example string_view vs const char*).
    // https://stackoverflow.com/a/3850120
    friend inline bool operator==(basic_string_view a,
                                  basic_string_view b) noexcept
    {
      return a.compare(b) == 0;
    }
    friend inline bool operator!=(basic_string_view a,
                                  basic_string_view b) noexcept
    {
      return a.compare(b) != 0;
    }

    friend inline bool operator<(basic_string_view a,
                                 basic_string_view b) noexcept
    {
      return a.compare(b) < 0;
    }

    friend inline bool operator>(basic_string_view a,
                                 basic_string_view b) noexcept
    {
      return a.compare(b) > 0;
    }

    friend inline bool operator<=(basic_string_view a,
                                  basic_string_view b) noexcept
    {
      return a.compare(b) <= 0;
    }

    friend inline bool operator>=(basic_string_view a,
                                  basic_string_view b) noexcept
    {
      return a.compare(b) >= 0;
    }

    friend std::ostream &operator<<(std::ostream &os, basic_string_view s)
    {
      std::ostream::sentry sentry{os};
      if (!sentry)
        return os;

      // Ideas from:
      // https://stackoverflow.com/q/39653508

      size_t padding = 0;
      char filler = os.fill();
      if (s.size() < os.width())
      {
        padding = os.width() - s.size();
      }

      bool align_left = os.flags() & std::ios_base::left;
      if (padding > 0 && !align_left)
      {
        while (padding--)
          os.put(filler);
      }
      os.write(s.data(), s.size());
      if (padding > 0 && align_left)
      {
        while (padding--)
          os.put(filler);
      }

      os.width(0);
      return os;
    }

  private:
    constexpr static size_type internal_strlen(const_pointer str)
    {
      return str ? traits_type::length(str) : 0;
    }

    static constexpr size_type kMaxSize = std::numeric_limits<size_type>::max();
    const_pointer data_;
    size_type len_;
  };

  template <class CharT, class Traits>
  constexpr typename basic_string_view<CharT, Traits>::size_type
      basic_string_view<CharT, Traits>::npos;

  template <class CharT, class Traits>
  constexpr typename basic_string_view<CharT, Traits>::size_type
      basic_string_view<CharT, Traits>::kMaxSize;

  using string_view = basic_string_view<char>;
  using u16string_view = basic_string_view<char16_t>;
  using u32string_view = basic_string_view<char32_t>;
  using wstring_view = basic_string_view<wchar_t>;

  string_view operator"" _sv(const char *str, std::size_t len) noexcept
  {
    return string_view(str, len);
  }
  u16string_view operator"" _sv(const char16_t *str, std::size_t len) noexcept
  {
    return u16string_view(str, len);
  }
  u32string_view operator"" _sv(const char32_t *str, std::size_t len) noexcept
  {
    return u32string_view(str, len);
  }
  wstring_view operator"" _sv(const wchar_t *str, std::size_t len) noexcept
  {
    return wstring_view(str, len);
  }

} // namespace david

namespace std
{
  template <>
  struct hash<david::string_view>
  {
    size_t operator()(david::string_view s) const
    {
      // Extracted from
      // https://stackoverflow.com/a/19411888
      return std::_Hash_impl::hash(s.data(), s.length());
    }
  };
} // namespace std

#endif // LIBRARY_STRING_VIEW
       // // STRING_VIEW_H