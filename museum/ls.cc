#include <bits/stdc++.h>
using namespace std;

template <size_t maxn, typename t> struct ls : array<t, maxn>
{
    using typename array<t, maxn>::value_type;
    using typename array<t, maxn>::reference;
    using typename array<t, maxn>::const_reference;
    using typename array<t, maxn>::iterator;
    using typename array<t, maxn>::const_iterator;
    using typename array<t, maxn>::size_type;
    using typename array<t, maxn>::reverse_iterator;
    using typename array<t, maxn>::const_reverse_iterator;
    using array<t, maxn>::begin;
    using array<t, maxn>::cbegin;
    using array<t, maxn>::data;
    using array<t, maxn>::operator[];

    constexpr ls() : std::array<t, maxn>{}, n(maxn)
    {
    }

    size_t n;

    inline void resize(size_t val) noexcept
    {
        assert((size_t) 0 <= val and val <= maxn);
        n = max((size_t) 0, min(val, maxn));
        // n = val;
    }

    inline constexpr void fill(const value_type &v)
    {
        fill_n(begin(), size(), v);
    }

    inline constexpr iterator end() noexcept
    {
        return begin() + n;
    }

    inline constexpr const_iterator end() const noexcept
    {
        return begin() + n;
    }

    inline constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    inline constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    inline constexpr const_iterator cend() const noexcept
    {
        return end();
    }

    inline constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    inline constexpr size_type size() const noexcept
    {
        return n;
    }

    inline constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    inline constexpr reference back() noexcept
    {
        return *(end() - 1);
    }

    inline constexpr const_reference back() const noexcept
    {
        return *(end() - 1);
    }
};