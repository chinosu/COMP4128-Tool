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

using str                        = string;
using z                          = long long;
const z    inf                   = numeric_limits<z>::max();
const auto gtz                   = greater<z>();
using pz                         = pair<z, z>;

template <typename T> using vec  = vector<T>;
using vz                         = vec<z>;
using vpz                        = vec<pz>;
using vvz                        = vec<vz>;

template <typename T> using mset = multiset<T>;

using statset =
    tree<z, null_type, less<z>, rb_tree_tag, tree_order_statistics_node_update>;
using statmset = tree<pz, null_type, less<pz>, rb_tree_tag,
                      tree_order_statistics_node_update>;

#pragma endregion
#pragma region macros

#define var                    auto
#define ref                    auto &

#define all(x)                 x.begin(), x.end()
#define ascz(i, stop)          for (z i = 0; i < stop; i += 1)
#define asc(i, start, stop)    for (z i = start; i < stop; i += 1)
#define dsc(i, start, stop)    for (z i = start; i > stop; i -= 1)
#define forch(vars, container) for (ref vars : container)
#define imax(store, consider)  store = max(store, consider)
#define elif                   else if
#define mpair                  make_pair
#define mtup                   make_tuple

#define SIZE                   .size()
#define BEGIN                  .begin()
#define END                    .end()
#define REND                   .rend()
#define RBEGIN                 .rbegin()
#define FRONT                  .front()
#define BACK                   .back()

#define zin                    cin >>
#define zout                   cout <<
#define zerr                   cerr <<
#define sp                     ' '
#define nl                     '\n'

#define sar(n_reserve, n, t, name)                                             \
    static array<t, n_reserve> __static_##name;                                \
    span<t>                    name = span(__static_##name).subspan(0, n);
#define sarz(n_reserve, n, name)                                               \
    static array<z, n_reserve> __static_##name;                                \
    span<z>                    name = span(__static_##name).subspan(0, n);

#define ALL      cin.tie
#define YOUR     (nullptr)
#define CONTESTS ;
#define ARE      cin.sync_with_stdio
#define BELONG   (false);
#define TO       cout << fixed << setprecision(9)
#define US       ;

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

// template <size_t N, typename T> void input_a(z n, arr<N, T> &a)
// {
//     ascz(i, n) cin >> a[i];
// }

// template <size_t N, typename T, typename U>
// void input_ap(z n, arr<N, pair<T, U>> &a)
// {
//     ascz(i, n) cin >> a[i].first >> a[i].second;
// }

// template <size_t N, size_t M, typename T>
// void input_aa(z n, z m, arr<N, arr<M, T>> &a)
// {
//     ascz(i, n) ascz(j, m) cin >> a[i][j];
// }

// template <typename T> void input_v(vec<T> &v)
// {
//     ascz(i, size(v)) cin >> v[i];
// }

// template <typename T, typename U> void input_vp(vec<pair<T, U>> &v)
// {
//     ascz(i, size(v)) cin >> v[i].first >> v[i].second;
// }

// template <typename T> void input_vv(vec<vec<T>> &v)
// {
//     ascz(i, size(v)) ascz(j, size(v[i])) cin >> v[i][j];
// }

#pragma endregion
#pragma region algos

var mq_in(deque<z> &mq, z x)
{
    while (size(mq) && mq.back() > x) mq.pop_back();
    mq.push_back(x);
}

var mq_er(deque<z> &mq, z x)
{
    if (size(mq) && mq.front() == x) mq.pop_front();
}

#pragma endregion

const z N = -19'06'2025;

int main()
{
    ALL YOUR CONTESTS ARE BELONG TO US;
    print("‧₊˚ ⋅");
}
