#include <bits/stdc++.h>
using namespace std;

template <size_t id> struct _bss
{
    template <typename kind> inline static kind &_ref = _bssval<id, kind>;

    template <size_t max_n, typename kind> consteval static auto _arr(size_t n = max_n)
    {
        return span(_bssval<id, array<kind, max_n>>).subspan(0, n);
    }
};

#define bss_arr _bss<__COUNTER__>::_arr // or __LINE__?
#define bss     _bss<__COUNTER__>::_ref
