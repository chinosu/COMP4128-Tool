#include <bits/stdc++.h>
using namespace std;

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
