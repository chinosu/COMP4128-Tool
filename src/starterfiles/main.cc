#pragma optimization_level 3
#pragma GCC optimize("Ofast,no-stack-protector,unroll-loops,fast-math,O3")
#include <bits/stdc++.h>
using namespace std;

template <typename t, auto s> struct dsu
{
    t parent[s];
    t rank[s];

    constexpr dsu()
    {
        iota(parent, parent + s, 0);
        fill(rank, rank + s, 1);
    }

    inline int find(t i)
    {
        while (parent[i] != i) i = parent[i] = parent[parent[i]];
        return i;
    }

    inline bool unite(t i, t j)
    {
        t pi = find(i), pj = find(j);
        if (pi == pj) return false;
        if (rank[pi] < rank[pj]) swap(pi, pj);
        parent[pj]  = pi;
        rank[pi]   += pj;
        return true;
    }

    inline bool same(t i, t j)
    {
        return find(i) == find(j);
    }

    inline int size(t i)
    {
        return rank[find(i)];
    }
};

template <typename t> struct minque
{
    deque<t> q;

    inline void add(t x)
    {
        while (q.size() and x < q.back()) q.pop_back();
        q.emplace_back(x);
    }

    inline void remove(t x)
    {
        if (q.size() and x == q.front()) q.pop_front();
    }
};

template <typename t, auto n> inline auto compress(t (&at)[n], t (&revert)[n])
{
    copy(at, at + n, revert);
    sort(revert, revert + n);
    int len = unique(revert, revert + n) - revert;
    for (int i = 0; i < n; i += 1) at[i] = lower_bound(revert, revert + len, at[i]) - revert;
    return len;
}

template <int n, int m, typename f, f flowinf> struct flow
{
    int tot = 1, s = 0, t = 0;
    int hd[n]{}, cur[n]{}, dis[n]{};
    int q[n]{}, qhd = 0, qtl = 0;

    struct
    {
        int to, next;
        f   cap;
    } e[m << 1 | 1]{};

    inline void clear()
    {
        tot = 1;
        fill(hd, hd + n, 0);
    }

    inline void add(int u, int v, f w)
    {
        e[++tot].next = hd[u], hd[u] = tot, e[tot].to = v, e[tot].cap = w;
        e[++tot].next = hd[v], hd[v] = tot, e[tot].to = u, e[tot].cap = 0;
    }

    inline bool bfs()
    {
        copy(hd, hd + n, cur);
        fill(dis, dis + n, -1);
        q[qhd = qtl = 1] = s;
        dis[s]           = 0;
        while (qhd <= qtl)
        {
            int u = q[qhd++];
            for (int i = hd[u], v; i; i = e[i].next)
            {
                if (dis[v = e[i].to] == -1 and e[i].cap != 0)
                {
                    dis[v]   = dis[u] + 1;
                    q[++qtl] = v;
                }
            }
        }
        return dis[t] != -1;
    }

    f dfs(int u, f rem)
    {
        if (u == t) return rem;
        f flow = 0;
        for (int i = cur[u], v; i and rem; i = e[i].next)
        {
            cur[u] = i;
            v      = e[i].to;
            f nw   = min(rem, e[i].cap);
            if (nw != 0 and dis[v] == dis[u] + 1)
            {
                int ret       = dfs(v, nw);
                flow         += ret;
                rem          -= ret;
                e[i].cap     -= ret;
                e[i ^ 1].cap += ret;
            }
        }
        if (flow == 0) dis[u] = -1;
        return flow;
    }

    f dinic(int source, int sink)
    {
        s = source, t = sink;
        f flow = 0;
        while (bfs()) flow += dfs(s, flowinf);
        return flow;
    }
};

constexpr inline bool isprime(auto x)
{
    if (x <= 1) return false;
    for (auto f = 2; f * f <= x; f += 1)
    {
        if (x % f == 0) return false;
    }
    return true;
}

template <typename t, auto n> struct primesieve
{
    t lp[n + 1]{};

    constexpr primesieve()
    {
        for (auto i = 2; i <= n; i += 1) lp[i] = i;
        for (auto i = 2; i * i <= n; i += 1)
        {
            if (lp[i] != i) continue;
            for (int j = i * i; j <= n; j += i)
            {
                if (lp[j] == j) lp[j] = i;
            }
        }
    }

    constexpr inline pair<vector<t>, vector<t>> factors(t x)
    {
        pair<vector<t>, vector<t>> ret;
        while (x)
        {
            if (ret.first.size() and ret.first.back() == lp[x]) ret.second.back() += 1;
            else ret.first.emplace_back(lp[x]), ret.second.emplace_back(1);
            x /= lp[x];
        }
        return ret;
    }
};

template <typename t, auto m> struct mod
{
    t val;

    constexpr mod(t v) : val((v % m + m) % m)
    {
    }

    constexpr inline auto operator+(const mod &o) const
    {
        return mod(val + o.val);
    }

    constexpr inline auto operator*(const mod &o) const
    {
        return mod(val * o.val);
    }

    constexpr inline auto operator+=(const mod &o)
    {
        val += o.val, val %= m;
        return *this;
    }

    constexpr inline auto operator*=(const mod &o)
    {
        val *= o.val, val %= m;
        return *this;
    }

    constexpr inline auto pow(auto p) const
    {
        mod x  = *this, ret;
        ret   += 1;
        while (0 < p)
        {
            if (p & 1) ret *= x;
            x *= x;
            p /= 2;
        }
        return ret;
    }

    constexpr inline auto inv() const
    {
        static_assert(isprime(m));
        return pow(m - 2);
    }

    constexpr inline operator t() const
    {
        return val;
    }
};

template <typename t> struct matrix
{
    int               n, m;
    vector<vector<t>> mat;

    matrix(int r, int c, t val = 0) : n(r), m(c), mat(r, vector<t>(c, val))
    {
    }

    inline vector<t> &operator[](size_t i)
    {
        return mat[i];
    }

    inline const vector<t> &operator[](size_t i) const
    {
        return mat[i];
    }

    static inline matrix identity(int n)
    {
        matrix m(n, n);
        for (int i = 0; i < n; ++i) m[i][i] = 1;
        return m;
    }

    friend inline matrix operator*(const matrix &a, const matrix &b)
    {
        assert(a.m == b.n);
        matrix c(a.n, b.m);
        for (auto i = 0; i < a.n; ++i)
        {
            for (auto k = 0; k < a.m; ++k)
            {
                auto aik = a[i][k];
                if (!aik) continue;
                for (auto j = 0; j < b.m; ++j)
                {
                    c.mat[i][j] += a[i][k] * b[k][j];
                }
            }
        }
        return c;
    }

    inline matrix pow(auto exp) const
    {
        assert(n == m);
        matrix a    = identity(n);
        matrix base = *this;
        while (exp > 0)
        {
            if (exp & 1) a = a * base;
            base   = base * base;
            exp  >>= 1;
        }
        return a;
    }
};

template <auto n> struct directed_cycle
{
    const char unseen = 0, active = 1, complete = 2;
    char       status[n]{};

    auto has_cycle(auto u, auto edges)
    {
        fill(status, status + n, 0);
        if (status[u] != unseen) return false;
        status[u] = active;
        for (auto v : edges)
        {
            if (status[v] == active or has_cycle(v)) return true;
        }
        status[u] = complete;
        return false;
    }
};

template <typename t, auto n> struct segtree
{
    t a[4 * n]{};

    void build(auto a0, auto comb, int u = 0, int l = 0, int r = n - 1)
    {
        if (l == r)
        {
            a[u] = a0[l];
            return;
        }
        int m = (l + r) >> 1;
        build(a0, comb, (u << 1) + 1, l, m);
        build(a0, comb, (u << 1) + 2, m + 1, r);
        comb(/* ref */ a[u], a[(u << 1) + 1], a[(u << 1) + 2]);
    }

    void set(int i, t x, auto comb, int u = 0, int l = 0, int r = n - 1)
    {
        if (l == r)
        {
            a[u] = x;
            return;
        }
        int m = (l + r) >> 1;
        if (i <= m) set(i, x, comb, (u << 1) + 1, l, m);
        else set(i, x, comb, (u << 1) + 2, m + 1, r);
        comb(/* ref */ a[u], a[(u << 1) + 1], a[(u << 1) + 2]);
    }

    void get(auto &ret, int i, int j, auto acc, int u = 0, int l = 0, int r = n - 1)
    {
        if (i <= l and r <= j)
        {
            acc(/* ref */ ret, a[u]);
            return;
        }
        int m = (l + r) >> 1;
        if (i <= m) get(ret, i, j, acc, (u << 1) + 1, l, m);
        if (m < j) get(ret, i, j, acc, (u << 1) + 2, m + 1, r);
    }
};

template <typename t, t ini = t{}, typename first, typename... rest> auto mvec(first f, rest... r)
{
    static_assert(is_integral_v<first>);
    if constexpr (sizeof...(rest) == 0) return vector<t>(f, ini);
    else return vector<decltype(mvec<t, ini>(r...))>(f, mvec<t, ini>(r...));
}

inline void in(auto &...x)
{
    ((cin >> x), ...);
}

inline void out(auto... x)
{
    ((cout << x), ...);
}

inline void err(auto... x)
{
    ((cerr << x), ...);
}

inline void imax(auto &a, const auto &b)
{
    a = max(a, b);
}

inline void imin(auto &a, const auto &b)
{
    a = min(a, b);
}

inline bool wthn(const auto left, const auto mid, const auto right)
{
    return left <= mid and mid < right;
}

template <typename t> inline constexpr decltype(auto) logceil(t x)
{
    return bit_width(x - 1);
}

constexpr unsigned long long operator""_K(unsigned long long x)
{
    return x * 1'000;
}

constexpr unsigned long long operator""_M(unsigned long long x)
{
    return x * 1'000'000;
}

constexpr unsigned long long operator""_B(unsigned long long x)
{
    return x * 1'000'000'000;
}

// #include <ext/pb_ds/assoc_container.hpp>
// #include <ext/pb_ds/hash_policy.hpp>
// #include <ext/pb_ds/list_update_policy.hpp>
// #include <ext/pb_ds/tag_and_trait.hpp>
// #include <ext/pb_ds/tree_policy.hpp>
// #include <ext/pb_ds/trie_policy.hpp>
// using namespace __gnu_pbds;
// using statset                    = tree<z, null_type, less<z>, rb_tree_tag, tree_order_statistics_node_update>;
// using statmset                   = tree<pz, null_type, less<pz>, rb_tree_tag, tree_order_statistics_node_update>;

using z                          = long long;
using pz                         = pair<z, z>;
using tz                         = tuple<z, z, z>;
const z maxz                     = numeric_limits<z>::max();
const z nil                      = 0;
using str                        = string;
template <typename t> using vec  = vector<t>;
template <typename t> using heap = priority_queue<t, vec<t>, greater<t>>;
using vz                         = vec<z>;
using vvz                        = vec<vz>;
using vvvz                       = vec<vvz>;
using vpz                        = vec<pz>;
using vtz                        = vec<tz>;
const char sp                    = ' ';
const char nl                    = '\n';

#define var                 auto
#define ref                 auto &
#define perm                static constinit
#define ascz(i, stop)       for (z i = 0; i < stop; i += 1)
#define asc(i, start, stop) for (z i = start; i < stop; i += 1)
#define dsc(i, start, stop) for (z i = start; stop < i; i -= 1)
#define scan(...)                                                                                                      \
    __VA_ARGS__;                                                                                                       \
    in(__VA_ARGS__)

#ifndef print
#define print(...)
#endif

int main()
{
    cin.tie(nullptr);
    ios::sync_with_stdio(false);
    cout << fixed << setprecision(9);
    const z inf = maxz;
}
