#pragma optimization_level 3
#pragma GCC optimize("Ofast,no-stack-protector,unroll-loops,fast-math,O3")
#include <bits/stdc++.h>
using namespace std;
namespace rn = std::ranges;
namespace vi = std::views;

template <typename t, auto s> struct dsu
{
    t parent[s];
    t rank[s];

    constexpr dsu()
    {
        iota(parent, parent + s, 0);
        fill(rank, rank + s, 1);
    }

    int find(t i)
    {
        while (parent[i] != i) i = parent[i] = parent[parent[i]];
        return i;
    }

    bool unite(t i, t j)
    {
        t pi = find(i), pj = find(j);
        if (pi == pj) return false;
        if (rank[pi] < rank[pj]) swap(pi, pj);
        parent[pj]  = pi;
        rank[pi]   += pj;
        return true;
    }

    bool same(t i, t j)
    {
        return find(i) == find(j);
    }

    int size(t i)
    {
        return rank[find(i)];
    }
};

template <typename t> struct minque
{
    deque<t> q;

    void add(t x)
    {
        while (q.size() and x < q.back()) q.pop_back();
        q.emplace_back(x);
    }

    void remove(t x)
    {
        if (q.size() and x == q.front()) q.pop_front();
    }
};

template <typename t, auto n> auto compress(t (&at)[n], t (&revert)[n])
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

    void clear()
    {
        tot = 1;
        fill(hd, hd + n, 0);
    }

    void add(int u, int v, f w)
    {
        e[++tot].next = hd[u], hd[u] = tot, e[tot].to = v, e[tot].cap = w;
        // change `0` to `w` to make network undirected
        e[++tot].next = hd[v], hd[v] = tot, e[tot].to = u, e[tot].cap = 0;
    }

    bool bfs()
    {
        copy(hd, hd + n, cur);
        fill(dis, dis + n, -1);
        q[qhd = qtl = 0] = s;
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
                f ret         = dfs(v, nw);
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

constexpr bool isprime(auto x)
{
    if (x <= 1) return false;
    for (auto f = 2; f * f <= x; f += 1)
    {
        if (x % f == 0) return false;
    }
    return true;
}

template <typename t> vector<t> primefactors(t x)
{
    vector<t> re;
    for (t f = 2; f * f <= x; f++)
    {
        for (; x % f == 0; x /= f) re.push_back(f);
    }
    if (x != 1) re.push_back(x);
    return re;
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

    constexpr pair<vector<t>, vector<t>> factors(t x)
    {
        pair<vector<t>, vector<t>> ret;
        while (2 <= x)
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

    constexpr mod(t v = 0) : val((v % m + m) % m)
    {
    }

    constexpr mod &operator+=(const mod &o)
    {
        return val += o.val, val %= m, *this;
    }

    constexpr mod &operator*=(const mod &o)
    {
        return val *= o.val, val %= m, *this;
    }

    friend constexpr mod operator+(mod a, const mod &b)
    {
        return a += b, a;
    }

    friend constexpr mod operator*(mod a, const mod &b)
    {
        return a *= b, a;
    }

    constexpr auto pow(auto p) const
    {
        mod x  = *this, ret(0);
        ret   += 1;
        while (0 < p)
        {
            if (p & 1) ret *= x;
            x *= x, p /= 2;
        }
        return ret;
    }

    constexpr auto inv()
        requires(isprime(m))
    {
        return pow(m - 2);
    }

    friend istream &operator>>(istream &is, const mod &x)
    {
        if (is >> x.val) x.val = (x.val % m + m) % m;
        return is;
    }

    friend ostream &operator<<(ostream &os, const mod &x)
    {
        return os << x.val;
    }

    explicit constexpr operator t() const
    {
        return val;
    }

    template <auto n> friend void factorial_table(auto &f, auto &finv)
    {
        f[0] = finv[0] = 1;
        for (size_t i = 1; i < n; i += 1) f[i] = i * f[i - 1];
        for (size_t i = 1; i < n; i += 1) finv[i] = f[i].inv();
    }
};

template <typename t> struct matrix
{
    int               n, m;
    vector<vector<t>> mat;

    matrix(int r, int c, t val = 0) : n(r), m(c), mat(r, vector<t>(c, val))
    {
    }

    vector<t> &operator[](size_t i)
    {
        return mat[i];
    }

    const vector<t> &operator[](size_t i) const
    {
        return mat[i];
    }

    static matrix identity(int n)
    {
        matrix m(n, n);
        for (int i = 0; i < n; ++i) m[i][i] = 1;
        return m;
    }

    friend matrix operator*(const matrix &a, const matrix &b)
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

    matrix pow(auto exp) const
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

// template <typename t> vector<t> permutation_matrix_pow(vector<t> &p, t k)
// {
//     t                 n = p.size();
//     vector<t>         re(n, -1), vis(n, 0), id(n), pos(n);
//     vector<vector<t>> cycles;
//     for (t i = 0; i < n; i += 1)
//     {
//         if (vis[i]) continue;
//         cycles.emplace_back();
//         for (t x = i; !vis[x]; x = p[x])
//         {
//             vis[x] = 1;
//             pos[x] = cycles.back().size();
//             id[x]  = cycles.size();
//             cycles.back().emplace_back(x);
//         }
//     }
//     for (auto &c : cycles)
//     {
//         t l = c.size(), s = (k % l + l) % l;
//         for (t i = 0; i < l; i += 1)
//         {
//             t at = c[i], to = c[(i + s) % l];
//             re[at] = to;
//         }
//     }
//     return re;
// }

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

const double eps = 1e-9;

template <integral t> bool zero(t x)
{
    return x == 0;
}

template <floating_point t> bool zero(t x)
{
    return fabs(x) <= eps;
}

template <typename t> struct pos
{
    t x;
    t y;

    pos operator()(pos u)
    {
        return pos(u.x - x, u.y - y);
    }

    friend pos operator-(pos u, pos v)
    {
        return pos(u.x - v.x, u.y - v.y);
    }

    friend t operator*(pos u)
    {
        return sqrt(pow(u.x, 2) + pow(u.y, 2));
    }

    friend t operator*(pos u, pos v)
    {
        return u.x * v.y - u.y * v.x;
    }

    friend int orient(pos u, pos v, pos w)
    {
        auto c = u(v) * u(w);
        if (zero(c)) return 0;
        if (c > 0) return 1;  // left
        if (c < 0) return -1; // right
        return 0;
    }

    friend bool collinear(pos a, pos b, pos c, pos d)
    {
        return zero(a(b) * a(c)) and zero(a(b) * a(d));
    }

    friend bool intersect(pos a, pos b, pos c, pos d)
    {
        if (collinear(a, b, c, d))
        {
            return max({*a(b), *a(c), *a(d), *b(c), *b(d), *c(d)}) < *a(b) + *c(d) + eps;
        }
        return orient(a, b, c) * orient(a, b, d) <= 0 and orient(c, d, a) * orient(c, d, b) <= 0;
    }

    friend t area(vector<pos> &p)
    {
        // triangle
        t ret = 0;
        for (int i = 1; i < p.size() - 1; i += 1) ret += p[0](p[i]) * p[0](p[i + 1]);
        return ret / 2;
    }

    friend t _area(vector<pos> &p)
    {
        // trapezoid
        t   ret = 0;
        int n   = p.size();
        for (int i = 0; i < n; i += 1) ret += (p[i].y + p[(i + 1) % n].y) * (p[(i + 1) % n].x - p[i].x);
        return ret / 2;
    }

    friend vector<pos> convexhull(vector<pos> p)
    {
        sort(p.begin(), p.end());
        vector<pos> v;
        int         s = 0;
        for (int i = 0; i < p.size(); i += 1)
        {
            // change `<= 0` to `< 0` to keep collinear points
            while (s + 2 <= v.size() and orient(v[v.size() - 2], v[v.size() - 1], p[i]) <= 0) v.pop_back();
            v.push_back(p[i]);
        }
        s = v.size() - 1;
        for (int i = p.size() - 2; 0 <= i; i -= 1)
        {
            // change `<= 0` to `< 0` to keep collinear points
            while (s + 2 <= v.size() and orient(v[v.size() - 2], v[v.size() - 1], p[i]) <= 0) v.pop_back();
            v.push_back(p[i]);
        }
        if (v.size()) v.pop_back();
        return v;
    }

    auto operator<=>(const pos &o) const = default;
};

// template <typename t> struct rat
// {
//     t n, d;

// constexpr rat() : n(0), d(1)
// {
// }

// constexpr rat(t n0, t d0 = 1) : n(n0), d(d0)
// {
//     if (d < 0) n *= -1, d *= -1;
// }

// rat operator+(const rat &o)
// {
//     return rat(n * o.d + o.n * d, d * o.d);
// }

// rat operator-(const rat &o)
// {
//     return rat(n * o.d - o.n * d, d * o.d);
// }

// rat operator*(const rat &o)
// {
//     return rat(n * o.n, d * o.d);
// }

// rat operator/(const rat &o)
// {
//     return rat(n * o.d, d * o.n);
// }

// auto operator<=>(const rat &o)
// {
//     return __int128(n) * __int128(o.d) <=> __int128(o.n) * __int128(d);
// }

// auto normalize()
// {
//     t g  = gcd(llabs(n), d);
//     n /= g, d /= g;
// }
// };

struct polar
{
    using i128  = __int128;

    long long u = 0, v = 0;
    i128      r2      = 0;
    bool      origin  = true;

    constexpr polar() = default;

    constexpr polar(long long x, long long y)
    {
        if (!(origin = (x == 0 and y == 0)))
        {
            r2 = i128(x) * i128(x) + i128(y) * i128(y);
            u = x / igcd(x, y), v = y / igcd(x, y);
        }
    }

    friend constexpr bool operator==(const polar &, const polar &) = default;

    friend constexpr strong_ordering operator<=>(const polar &a, const polar &b)
    {
        if (a.origin and b.origin) return strong_ordering::equivalent;
        if (a.origin) return strong_ordering::less;
        if (b.origin) return strong_ordering::greater;
        if (a.half() != b.half()) return a.half() ? strong_ordering::less : strong_ordering::greater;
        auto cross = i128(a.u) * i128(b.v) - i128(a.v) * i128(b.u);
        if (0 < cross) return strong_ordering::less;
        if (cross < 0) return strong_ordering::greater;
        if (a.r2 < b.r2) return strong_ordering::less;
        if (b.r2 < a.r2) return strong_ordering::greater;
        return strong_ordering::equivalent;
    }

  private:
    constexpr long long igcd(long long a, long long b) const
    {
        if (a < 0) a *= -1;
        if (b < 0) b *= -1;
        while (b)
        {
            auto t = a % b;
            a = b, b = t;
        }
        return a ? a : 1;
    }

    constexpr bool half() const
    {
        return v > 0 or v == 0 and u > 0;
    }
};

template <typename t0, typename t1, t1 inf> vector<t1> dijkstra(vector<vector<pair<t0, t1>>> a, t0 s)
{
    vector<t1>                                                    d(a.size(), inf);
    priority_queue<pair<t1, t0>, vector<pair<t1, t0>>, greater<>> q;
    q.emplace(0, s);
    while (q.size())
    {
        auto [du, u] = q.top();
        q.pop();
        if (d[u] != inf) continue;
        d[u] = du;
        for (auto [v, w] : a[u])
        {
            if (d[v] != inf) continue;
            q.emplace(du + w, v);
        }
    }
    return d;
}

template <integral t, t none = numeric_limits<t>::max()>
pair<vector<vector<t>>, vector<t>> scc_kosaraju(vector<vector<t>> &ad)
{
    t         n = ad.size();
    vector<t> order;
    order.reserve(n);
    vector<char> use(n, false);
    auto         dfs0 = [&](auto self, t u) -> void
    {
        use[u] = true;
        for (t v : ad[u])
        {
            if (!use[v]) self(self, v);
        }
        order.emplace_back(u);
    };
    for (t u = 0; u < n; u += 1)
    {
        if (!use[u]) dfs0(dfs0, u);
    }

    vector<vector<t>> adr(n);
    for (t u = 0; u < n; u += 1)
    {
        for (t v : ad[u]) adr[v].emplace_back(u);
    }

    vector<t> id(n, none);
    auto      dfs1 = [&](auto self, t u, t c) -> void
    {
        id[u] = c;
        for (t v : adr[u])
        {
            if (id[v] == none) self(self, v, c);
        }
    };
    t c = 0;
    for (t u : order | views::reverse)
    {
        if (id[u] == none) dfs1(dfs1, u, c++);
    }

    vector<vector<t>> dag(c);
    for (t u = 0; u < n; u += 1)
    {
        for (t v : ad[u])
        {
            if (id[u] != id[v]) dag[id[u]].emplace_back(id[v]);
        }
    }
    for (auto &v : dag) sort(v.begin(), v.end()), v.erase(unique(v.begin(), v.end()), v.end());
    return {dag, id};
}

template <integral t, t none = numeric_limits<t>::max()>
tuple<vector<vector<t>>, vector<pair<t, t>>, vector<t>> bridge_tree(vector<vector<t>> &ad)
{
    t                  n = ad.size(), c = 0;

    vector<t>          pre(n, none), low(n, 0), id(n, none);
    vector<char>       use(n, false);
    vector<pair<t, t>> bridges;
    auto               dfs = [&](auto self, t u, t p = none) -> void
    {
        use[u] = true, pre[u] = low[u] = c++;
        for (t v : ad[u])
        {
            if (p == v) continue;
            if (use[v]) low[u] = min(low[u], pre[v]);
            else
            {
                self(self, v, u);
                low[u] = min(low[u], low[v]);
                if (pre[u] < low[v]) bridges.emplace_back(u, v);
            }
        }
    };
    for (t u = 0; u < n; u += 1)
    {
        if (!use[u]) dfs(dfs, u);
    }

    auto isbridge = bridges;
    for (auto &[u, v] : isbridge)
    {
        if (v < u) swap(u, v);
    }
    sort(isbridge.begin(), isbridge.end());

    c = 0;
    for (t u = 0; u < n; u += 1)
    {
        if (id[u] != none) continue;
        deque<t> s;
        id[u] = c, s.emplace_back(u);
        while (s.size())
        {
            t v = s.front();
            s.pop_front();
            for (t w : ad[v])
            {
                if (id[w] != none) continue;
                if (binary_search(isbridge.begin(), isbridge.end(), v < w ? make_pair(v, w) : make_pair(w, v)))
                {
                    continue;
                }
                id[w] = c, s.emplace_back(w);
            }
        }
        c++;
    }

    vector<vector<t>> tree(c);
    for (auto [u, v] : bridges) tree[id[u]].emplace_back(id[v]);
    return {tree, bridges, id};
}

template <typename t>
constinit array<pair<t, t>, 4> directions{
    {{t(0), t(1)}, {t(0), t(-1)}, {t(1), t(0)}, {t(-1), t(0)}}
};

template <typename t, t ini = t{}, typename first, typename... rest> auto mvec(first f, rest... r)
{
    static_assert(is_integral_v<first>);
    if constexpr (sizeof...(rest) == 0) return vector<t>(f, ini);
    else return vector<decltype(mvec<t, ini>(r...))>(f, mvec<t, ini>(r...));
}

void in(auto &...x)
{
    ((cin >> x), ...);
}

int out(auto... x)
{
    ((cout << x), ...);
    return 0;
}

int err(auto... x)
{
    ((cerr << x), ...);
    return 0;
}

bool imax(auto &a, const auto &b)
{
    return a > b ? false : a = b, true;
}

bool imin(auto &a, const auto &b)
{
    return a < b ? false : a = b, true;
}

bool wthn(const auto left, const auto mid, const auto right)
{
    return left <= mid and mid < right;
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

namespace _lambda
{

template <typename t> constexpr t &&fwd(remove_reference_t<t> &x) noexcept
{
    return static_cast<t &&>(x);
}

template <typename t> constexpr t &&fwd(remove_reference_t<t> &&x) = delete;

template <int> constexpr array<char, 0> nth_arg() noexcept
{
    return {};
}

template <int n, class h, class... t> constexpr decltype(auto) nth_arg(h &&head, t &&...tail) noexcept
{
    if constexpr (n == 0) return _lambda::fwd<h>(head);
    else return nth_arg<n - 1>(_lambda::fwd<t>(tail)...);
}

template <int n, class... args> using nth_arg_t = decltype(_lambda::nth_arg<n>(declval<args>()...));

#define _lambda_req_param_1 _lambda::nth_arg_t<0, lambda_args...> _1
#define _lambda_req_param_2 _lambda_req_param_1, _lambda::nth_arg_t<1, lambda_args...> _2
#define _lambda_req_param_3 _lambda_req_param_2, _lambda::nth_arg_t<2, lambda_args...> _3
#define _lambda_req_param_4 _lambda_req_param_3, _lambda::nth_arg_t<3, lambda_args...> _4
#define _lambda_req_param_5 _lambda_req_param_4, _lambda::nth_arg_t<4, lambda_args...> _5
#define _lambda_req_param_6 _lambda_req_param_5, _lambda::nth_arg_t<5, lambda_args...> _6
#define _lambda_req_param_7 _lambda_req_param_6, _lambda::nth_arg_t<6, lambda_args...> _7
#define _lambda_req_param_8 _lambda_req_param_7, _lambda::nth_arg_t<7, lambda_args...> _8
#define _lambda_req(...)                                                                                               \
    requires(_lambda_req_param_8) {                                                                                    \
        __VA_ARGS__;                                                                                                   \
    }
#define _lambda_define_locals()                                                                                        \
    auto &&_1 = _lambda::nth_arg<0>(_lambda::fwd<lambda_args>(_args)...);                                              \
    auto &&_2 = _lambda::nth_arg<1>(_lambda::fwd<lambda_args>(_args)...);                                              \
    auto &&_3 = _lambda::nth_arg<2>(_lambda::fwd<lambda_args>(_args)...);                                              \
    auto &&_4 = _lambda::nth_arg<3>(_lambda::fwd<lambda_args>(_args)...);                                              \
    auto &&_5 = _lambda::nth_arg<4>(_lambda::fwd<lambda_args>(_args)...);                                              \
    auto &&_6 = _lambda::nth_arg<5>(_lambda::fwd<lambda_args>(_args)...);                                              \
    auto &&_7 = _lambda::nth_arg<6>(_lambda::fwd<lambda_args>(_args)...);                                              \
    auto &&_8 = _lambda::nth_arg<7>(_lambda::fwd<lambda_args>(_args)...)
#define lambda(...)                                                                                                    \
    [&]<typename... lambda_args>(lambda_args &&..._args) mutable noexcept -> decltype(auto)                            \
        requires _lambda_req                                                                                           \
    (__VA_ARGS__)                                                                                                      \
    {                                                                                                                  \
        _lambda_define_locals();                                                                                       \
        return (__VA_ARGS__);                                                                                          \
    }
} // namespace _lambda

#define perm                static constinit
#define ascz(i, stop)       for (z i = 0; i < stop; i += 1)
#define asc(i, start, stop) for (z i = start; i < stop; i += 1)
#define dsc(i, start, stop) for (z i = start; stop < i; i -= 1)
#define foreach(var, ...)   for (auto var : __VA_ARGS__)
#define scan(...)                                                                                                      \
    __VA_ARGS__;                                                                                                       \
    in(__VA_ARGS__)
#ifndef print
#define print(...)
#endif

using z                          = long long;
using pz                         = pair<z, z>;
using tz                         = tuple<z, z, z>;
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

int main()
{
    cin.tie(nullptr)->sync_with_stdio(false), cout << fixed << setprecision(9);
    const z inf = numeric_limits<z>::max();
}
