#pragma region yap

#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/hash_policy.hpp>
#include <ext/pb_ds/list_update_policy.hpp>
#include <ext/pb_ds/tag_and_trait.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/pb_ds/trie_policy.hpp>

#pragma optimization_level 3
#pragma GCC optimize("Ofast,no-stack-protector,unroll-loops,fast-math,O3")

using namespace std;
using namespace __gnu_pbds;

#pragma endregion
#pragma region types

using str                                    = string;
using z                                      = long long;
const z    inf                               = numeric_limits<z>::max();
const auto gtz                               = greater<z>();
using pz                                     = pair<z, z>;

template <typename T> using vec              = vector<T>;
using vz                                     = vec<z>;
using vpz                                    = vec<pz>;
using vvz                                    = vec<vz>;

template <typename T> using mset             = multiset<T>;

template <size_t n, typename kind> using arr = array<kind, n>;

using statset =
    tree<z, null_type, less<z>, rb_tree_tag, tree_order_statistics_node_update>;
using statmset = tree<pz, null_type, less<pz>, rb_tree_tag,
                      tree_order_statistics_node_update>;

#pragma endregion
#pragma region macros

#define var                 auto
#define ref                 auto &

#define all(x)              x.begin(), x.end()
#define ascz(i, stop)       for (z i = 0; i < stop; i += 1)
#define asc(i, start, stop) for (z i = start; i < stop; i += 1)
#define dsc(i, start, stop) for (z i = start; i > stop; i -= 1)
#define forch(item, items)  for (ref item : items)
#define ifnot(cond)         if (!(cond))
#define elif                else if
#define imin(a, b)          a = min(a, b);
#define imax(a, b)          a = max(a, b);
#define iamod(a, b, m)      a = (a + b) % m

#define SIZE                .size()
#define BEGIN               .begin()
#define END                 .end()
#define REND                .rend()
#define RBEGIN              .rbegin()
#define FRONT               .front()
#define BACK                .back()

#define out                 cout <<
#define err                 cerr <<
#define nl                  << '\n';

#define ALL                 cin.tie
#define YOUR                (nullptr)
#define CONTESTS            ;
#define ARE                 cin.sync_with_stdio
#define BELONG              (false);
#define TO                  cout << fixed << setprecision(9)
#define US                  ;

#pragma endregion
#pragma region debugprint

#ifdef __DEBUG__
void __p(int x)
{
    cerr << x;
}

void __p(long x)
{
    cerr << x;
}

void __p(long long x)
{
    cerr << x;
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
    var first = true;
    cerr << '[';
    for (const var &i : x)
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
    var first = true;
    cerr << '(';
    apply([&first](const auto &...args)
          { ((cerr << (first ? "" : ","), __p(args), first = false), ...); },
          t);
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

template <typename Head, typename... Tail>
void _p(const Head &H, const Tail &...T)
{
    __p(H);
    if (sizeof...(T)) cerr << ", ";
    _p(T...);
}

#define print(...)                                                             \
    cerr << "" << __LINE__ << " => [" << #__VA_ARGS__ << "] = [";              \
    _p(__VA_ARGS__);

#else
#define print(...)
#endif

#pragma endregion
#pragma region io

template <typename... T> void in(T &...a)
{
    ((cin >> a), ...);
}

#pragma endregion
#pragma region                          eternal/global

template <size_t _, typename kind> kind __globalval;

template <size_t id> struct __global
{
    template <size_t max_n, typename kind>
    consteval static auto __arr(size_t n = max_n)
    {
        return span(__globalval<id, arr<max_n, kind>>).subspan(0, n);
    }

    // template <typename kind>
    // static kind &__ref()
    // {
    //     return __globalval<id, kind>;
    // }
};

#define bss_arr __global<__COUNTER__>::__arr // or __LINE__?
// #define eternal __global<__COUNTER__>::__ref

#pragma endregion
#pragma region algos

struct dsu
{
    z  n;
    z *parent;
    z *rank;

    dsu(z n) : n(n)
    {
        parent               = static_cast<z *>(malloc(sizeof(z) * n));
        ascz(i, n) parent[i] = i;
        rank                 = static_cast<z *>(malloc(sizeof(z) * n));
        fill(rank, rank + n, 1);
    }

    ~dsu()
    {
        free(parent);
        free(rank);
    }

    inline z find(z i)
    {
        while (parent[i] != i) i = parent[i] = parent[parent[i]];
        return i;
    }

    inline bool unite(z i, z j)
    {
        z pi = find(i);
        z pj = find(j);
        if (pi == pj) return false;
        if (rank[pi] < rank[pj]) swap(pi, pj);
        parent[pj]  = pi;
        rank[pi]   += pj;
        return true;
    }

    inline bool same(z i, z j)
    {
        return find(i) == find(j);
    }

    inline int size(z i)
    {
        return rank[find(i)];
    }
};

struct minque
{
    deque<z> q;

    inline void add(z item)
    {
        while (q SIZE and q BACK > item) q.pop_back();
        q.push_back(item);
    }

    inline void remove(z item)
    {
        if (q SIZE and q FRONT == item) q.pop_front();
    }
};

#pragma endregion

int main()
{
    ALL YOUR CONTESTS ARE BELONG TO US;
    print("‧₊˚ ⋅");
}
