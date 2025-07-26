#include <bits/stdc++.h>
using namespace std;

template <typename t, size_t n, size_t... m> constexpr auto matrix(t ini = t{})
{
    if constexpr (sizeof...(m) == 0)
    {
        array<t, n> a{};
        fill(a.begin(), a.end(), ini);
        return a;
    }
    else
    {
        array<decltype(matrix<t, m...>(ini)), n> a{};
        fill(a.begin(), a.end(), matrix<t, m...>(ini));
        return a;
    }
}

template <typename t, size_t n, size_t... m> struct _matrix;

template <typename t, size_t n> struct _matrix<t, n>
{
    using type = array<t, N>;
};

template <typename t, size_t n, size_t m0, size_t... m> struct _matrix<t, n, m0, m...>
{
    using type = array<typename _matrix<t, m0, m...>::type, n>;
};

template <typename t, size_t n, size_t... m> using matrix_t = typename _matrix<t, n, m...>::type;
