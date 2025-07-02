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
#pragma region macros

#define let                 auto
#define ref                 auto &
#define uniq                static constinit
#define uniqz               static constinit z
#define infer               decltype(auto)

#define all(x)              x.begin(), x.end()
#define rall(x)             x.rbegin(), x.rend()
#define asc(i, start, stop) for (z i = start; i < stop; i += 1)
#define ascz(i, stop)       for (z i = 0; i < stop; i += 1)
#define asc1(i, stop)       for (z i = 1; i <= stop; i += 1)
#define dsc(i, start, stop) for (z i = start; i > stop; i -= 1)
#define ifnot(cond)         if (!(cond))
#define elif                else if

#define SIZE                size()
#define BEGIN               begin()
#define END                 end()
#define REND                rend()
#define RBEGIN              rbegin()
#define FRONT               front()
#define BACK                back()
#define DATA                data()

#define ALL                 cin.tie
#define YOUR                (nullptr)
#define CONTESTS            ;
#define ARE                 cin.sync_with_stdio
#define BELONG              (false);
#define TO                  cout << fixed << setprecision(9)
#define US                  ;

#pragma endregion
#pragma region types

using str     = string;
using z       = long long;
const z   inf = numeric_limits<z>::max();
const z   nil = 0;
const let gtz = greater<z>();
using pz      = pair<z, z>;

template <size_t maxn, typename kind> struct ray : array<kind, maxn>
{
    using typename array<kind, maxn>::value_type;
    using typename array<kind, maxn>::reference;
    using typename array<kind, maxn>::const_reference;
    using typename array<kind, maxn>::iterator;
    using typename array<kind, maxn>::const_iterator;
    using typename array<kind, maxn>::size_type;
    using typename array<kind, maxn>::reverse_iterator;
    using typename array<kind, maxn>::const_reverse_iterator;
    using array<kind, maxn>::begin;
    using array<kind, maxn>::cbegin;
    using array<kind, maxn>::data;
    using array<kind, maxn>::operator[];

    constexpr ray() : std::array<kind, maxn>{}, n(maxn) {}

    size_t n;

    inline void resize(size_t val) noexcept
    {
        assert((size_t) 0 <= val and val < maxn);
        n = max((size_t) 0, min(val, maxn));
        // n = val;
    }

    inline constexpr void fill(const value_type &v) { fill_n(begin(), size(), v); }

    inline constexpr iterator end() noexcept { return begin() + n; }

    inline constexpr const_iterator end() const noexcept { return begin() + n; }

    inline constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    inline constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    inline constexpr const_iterator cend() const noexcept { return end(); }

    inline constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

    inline constexpr size_type size() const noexcept { return n; }

    inline constexpr bool empty() const noexcept { return size() == 0; }

    inline constexpr reference back() noexcept { return *(end() - 1); }

    inline constexpr const_reference back() const noexcept { return *(end() - 1); }
};

template <size_t maxn> using rz               = ray<maxn, z>;
template <size_t maxn, size_t maxm> using rrz = ray<maxn, rz<maxm>>;
template <size_t maxn> using rpz              = ray<maxn, pz>;

template <typename kind> using vec            = vector<kind>;
using vz                                      = vec<z>;
using vpz                                     = vec<pz>;
using vvz                                     = vec<vz>;

template <typename kind> using mset           = multiset<kind>;
using statset  = tree<z, null_type, less<z>, rb_tree_tag, tree_order_statistics_node_update>;
using statmset = tree<pz, null_type, less<pz>, rb_tree_tag, tree_order_statistics_node_update>;

#pragma endregion
#pragma region functions

template <typename kind>
concept istream_able = requires(istream &is, kind &k) {
    { is >> k } -> same_as<istream &>;
};

template <istream_able... kind> inline void in(kind &...a) { ((cin >> a), ...); }

template <typename kind>
concept ostream_able = requires(ostream &os, const kind &k) {
    { os << k } -> same_as<ostream &>;
};

const let sp = ' ';
const let nl = '\n';

template <ostream_able... kind> inline void out(kind... a) { ((cout << a), ...); }

template <ostream_able... kind> inline void err(kind... a) { ((cerr << a), ...); }

template <typename kind> inline void imax(kind &a, const kind &b) { a = max(a, b); }

template <typename kind> inline void imin(kind &a, const kind &b) { a = min(a, b); }

template <typename kind> inline void iamod(kind &a, const kind &b, const kind &m) { a = (a + b) % m; }

template <typename kind> inline constexpr kind ifexp(bool cond, kind a, kind b) { return cond ? a : b; }

template <typename kind> inline constexpr bool within(kind left, kind mid, kind right)
{
    return left <= mid and mid < right;
}

template <typename kind> inline constexpr infer logceil(kind x) { return bit_width(x - 1); }

template <typename kind> inline infer get0(const kind &k) { return get<0>(k); }

template <typename kind> inline infer get1(const kind &k) { return get<1>(k); }

template <typename kind> inline infer get2(const kind &k) { return get<2>(k); }

template <typename kind> inline infer get3(const kind &k) { return get<3>(k); }

template <typename kind> inline infer get4(const kind &k) { return get<4>(k); }

constexpr unsigned long long operator"" _K(unsigned long long item) { return item * 1'000; }

constexpr unsigned long long operator"" _M(unsigned long long item) { return item * 1'000'000; }

#pragma endregion
#pragma region debugprint

#ifdef __DEBUG__
void __p(int x) { cerr << x; }

void __p(long x) { cerr << x; }

void __p(long long x) { cerr << x; }

void __p(unsigned x) { cerr << x; }

void __p(unsigned long x) { cerr << x; }

void __p(unsigned long long x) { cerr << x; }

void __p(float x) { cerr << x; }

void __p(double x) { cerr << x; }

void __p(long double x) { cerr << x; }

void __p(char x) { cerr << '\'' << x << '\''; }

void __p(const char *x) { cerr << '"' << x << '"'; }

void __p(const string &x) { cerr << '"' << x << '"'; }

void __p(bool x) { cerr << (x ? "true" : "false"); }

template <typename A> void __p(const A &x);
template <typename A, typename B> void __p(const pair<A, B> &p);
template <typename... A> void __p(const tuple<A...> &t);
template <typename T> void __p(stack<T> s);
template <typename T> void __p(queue<T> q);
template <typename T, typename... U> void __p(priority_queue<T, U...> q);
template <size_t N> void __p(bitset<N> q);

template <typename A> void __p(const A &x)
{
    let first = true;
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
    let first = true;
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

template <size_t N> void __p(bitset<N> q) { cerr << q; }

void _p() { cerr << "]\n"; }

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

#pragma endregion
#pragma region algo

template <size_t maxn> struct dsu
{
    z            n;
    ray<maxn, z> parent;
    ray<maxn, z> rank;

    inline void ini(z n)
    {
        parent.resize(n);
        rank.resize(n);
        iota(all(parent), 0);
        fill(all(rank), 1);
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

    inline bool same(z i, z j) { return find(i) == find(j); }

    inline int size(z i) { return rank[find(i)]; }
};

struct minque
{
    deque<z> q;

    inline void add(z item)
    {
        while (q.SIZE and q.BACK > item) q.pop_back();
        q.push_back(item);
    }

    inline void remove(z item)
    {
        if (q.SIZE and q.FRONT == item) q.pop_front();
    }
};

template <size_t maxn> struct coord_compress
{
    ray<maxn, z> origin;
    ray<maxn, z> small;

    inline void press(span<z> items)
    {
        origin.resize(items.SIZE);
        small.resize(items.SIZE);
        copy(all(items), origin.BEGIN);
        sort(all(origin));
        origin.resize(unique(all(origin)) - origin.BEGIN);
        ascz (i, items.SIZE) small[i] = lower_bound(all(origin), items[i]) - origin.BEGIN;
    }
};

struct _TODO_range_tree
{
    z  n;
    z *oak;
};

#pragma endregion

int main()
{
    ALL YOUR CONTESTS ARE BELONG TO US;
    print("‧₊˚ ⋅");
}
