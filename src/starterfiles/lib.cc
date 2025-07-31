#include <bits/stdc++.h>
using namespace std;

#define roundup(val, by) (((val) + (by) - 1) / (by) * (by))

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
