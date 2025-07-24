#pragma optimization_level 3
#pragma GCC optimize("Ofast,no-stack-protector,unroll-loops,fast-math,O3")
#include <bits/stdc++.h>
using namespace std;

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
template <typename t> using gt   = greater<t>;
template <typename t> using vec  = vector<t>;
template <typename t> using heap = priority_queue<t, vec<t>, gt<t>>;
using vz                         = vec<z>;
using vvz                        = vec<vz>;
using vvvz                       = vec<vvz>;
using vpz                        = vec<pz>;
using vtz                        = vec<tz>;
template <typename t> using mset = multiset<t>;
const char sp                    = ' ';
const char nl                    = '\n';

#define var                 auto
#define ref                 auto &
#define perm                static constinit
#define all(x)              x.begin(), x.end()
#define ascz(i, stop)       for (z i = 0; i < stop; i += 1)
#define asc(i, start, stop) for (z i = start; i < stop; i += 1)
#define dsc(i, start, stop) for (z i = start; stop < i; i - = 1)
#define guard(cond)         if (!(cond))
#define main                                                                                                           \
    int main()                                                                                                         \
    {                                                                                                                  \
        cin.tie(nullptr);                                                                                              \
        cin.sync_with_stdio(false);                                                                                    \
        cout << fixed << setprecision(9);
#define niam }

// template <typename t, size_t n, typename fn> constexpr array<t, n> arrayfill(fn &&f)
// {
//     array<t, n> a{};
//     for (size_t i = 0; i < n; i += 1) a[i] = f(i);
//     return a;
// }

// template <typename t, size_t n, size_t... m> constexpr auto make_matrix(t ini = t{})
// {
//     if constexpr (sizeof...(m) == 0)
//     {
//         array<t, n> a{};
//         fill(a.begin(), a.end(), ini);
//         return a;
//     }
//     else
//     {
//         array<decltype(make_matrix<t, m...>(ini)), n> a{};
//         fill(a.begin(), a.end(), make_matrix<t, m...>(ini));
//         return a;
//     }
// }

// template <typename t, t ini = t{}, typename first, typename... rest> auto mvec(first f, rest... r)
// {
//     static_assert(is_integral_v<first>);
//     if constexpr (sizeof...(rest) == 0) return vector<t>(f, ini);
//     else return vector<decltype(mvec<t, ini>(r...))>(f, mvec<t, ini>(r...));
// }

inline void in(auto &...x)
{
    ((cin >> x), ...);
}

#define zin                                                                                                            \
    ({                                                                                                                 \
        z x;                                                                                                           \
        in(x);                                                                                                         \
        x;                                                                                                             \
    })

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

inline void iamod(auto &a, const auto &b, const auto &m)
{
    a = (a + b) % m;
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

#ifdef __DEBUG__
void __p(int x)
{
    if (x == numeric_limits<int>::max()) cerr << "∞";
    else if (x == -numeric_limits<int>::max()) cerr << "-∞";
    else cerr << x;
}

void __p(long x)
{
    cerr << x;
}

void __p(long long x)
{
    if (x == numeric_limits<long long>::max()) cerr << "∞";
    else if (x == -numeric_limits<long long>::max()) cerr << "-∞";
    else cerr << x;
}

void __p(unsigned x)
{
    cerr << x;
}

void __p(unsigned long x)
{
    cerr << x;
}

void __p(unsigned long long x)
{
    cerr << x;
}

void __p(float x)
{
    cerr << x;
}

void __p(double x)
{
    cerr << x;
}

void __p(long double x)
{
    cerr << x;
}

void __p(char x)
{
    cerr << '\'' << x << '\'';
}

void __p(const char *x)
{
    cerr << '"' << x << '"';
}

void __p(const string &x)
{
    cerr << '"' << x << '"';
}

void __p(bool x)
{
    cerr << (x ? "true" : "false");
}

template <typename A> void __p(const A &x);
template <typename A, typename B> void __p(const pair<A, B> &p);
template <typename... A> void __p(const tuple<A...> &t);
template <typename T> void __p(stack<T> s);
template <typename T> void __p(queue<T> q);
template <typename T, typename... U> void __p(priority_queue<T, U...> q);
template <size_t N> void __p(bitset<N> q);

template <typename A> void __p(const A &x)
{
    auto first = true;
    cerr << '[';
    for (const ref i : x)
    {
        cerr << (first ? "" : ","), __p(i);
        first = false;
    }
    cerr << ']';
}

template <typename A, typename B> void __p(const pair<A, B> &p)
{
    cerr << '(';
    __p(p.first);
    cerr << ",";
    __p(p.second);
    cerr << ')';
}

template <typename... A> void __p(const tuple<A...> &t)
{
    auto first = true;
    cerr << '(';
    apply([&first](const auto &...args) { ((cerr << (first ? "" : ","), __p(args), first = false), ...); }, t);
    cerr << ')';
}

template <typename T> void __p(stack<T> s)
{
    vec<T> v;
    while (!s.empty())
    {
        T t = s.top();
        v.push_back(t);
        s.pop();
    }
    reverse(all(v));
    __p(v);
}

template <typename T> void __p(queue<T> q)
{
    vec<T> v;
    while (!q.empty())
    {
        T t = q.front();
        v.push_back(t);
        q.pop();
    }
    __p(v);
}

template <typename T, typename... U> void __p(priority_queue<T, U...> q)
{
    vec<T> v;
    while (!q.empty())
    {
        T t = q.top();
        v.push_back(t);
        q.pop();
    }
    __p(v);
}

template <size_t N> void __p(bitset<N> q)
{
    cerr << q;
}

void _p()
{
    cerr << "]\n";
}

template <typename Head, typename... Tail> void _p(const Head &H, const Tail &...T)
{
    __p(H);
    if (sizeof...(T)) cerr << ", ";
    _p(T...);
}

#define print(...)                                                                                                     \
    do {                                                                                                               \
        cerr << "" << __LINE__ << " => [" << #__VA_ARGS__ << "] = [";                                                  \
        _p(__VA_ARGS__);                                                                                               \
    } while (0)
#else
#define print(...)
#endif

template <typename t, int s> struct dsu
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

template <typename t, int n> inline auto compress(t (&at)[n], t (&revert)[n])
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
    } e[m << 1]{};

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

/* ------------------------------- */ main; /* ------------------------------- */

perm pz directions[]{
    {-1, 0 },
    {1,  0 },
    {0,  -1},
    {0,  1 }
};
perm flow<90_K + 2, 360_K, z, maxz> f;

z                                   n = zin, s = 0, t = 1;
var                                 v = [&](z i, z j) { return 2 + i * n + j; };
vec<vec<char>>                      g(n, vec<char>(n));
print(n);
ascz(i, n) ascz(j, n) in(g[i][j]);
print(g);

ascz(i, n) ascz(j, n)
{
    guard(g[i][j] == '#') continue;
    if (i + j & 1) f.add(s, v(i, j), 1);
    else f.add(v(i, j), t, 1);
    for (ref[di, dj] : directions)
    {
        guard(wthn(nil, i + di, n)) continue;
        guard(wthn(nil, j + dj, n)) continue;
        if (i + j & 1) f.add(v(i, j), v(i + di, j + dj), 1);
    }
}

z ans = f.dinic(s, t);
out(ans, nl);

/* ------------------------------- */ niam; /* ------------------------------- */
