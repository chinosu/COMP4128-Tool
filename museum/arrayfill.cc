#include <bits/stdc++.h>
using namespace std;

template <typename t, size_t n, typename fn> constexpr array<t, n> arrayfill(fn &&f)
{
    array<t, n> a{};
    for (size_t i = 0; i < n; i += 1) a[i] = f(i);
    return a;
}
