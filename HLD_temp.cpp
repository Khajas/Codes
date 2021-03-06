#include <bits/stdc++.h>
#define MAXLVL 19
#define MAX 100005

using namespace std;

vector < pair<int,int> > v[MAX];
int child[MAX];
int depth[MAX];
pair <int,int> edges[MAX];
int base_array_val[MAX];
int par[MAXLVL][MAX];
int tree[4*MAX];

int chain_head[MAX];
int chain_size[MAX]; //number of nodes in a particular chain
int chain_index[MAX]; //chain_index[i] tells us the chain index of the ith node
int chain_pos[MAX]; //chain_pos[i] denotes position of the ith node in the chain_index[i]th chain
int pos_base_array[MAX];

int n;
int chain;
int base_array_ptr;

//Code for FAST INPUT :)
template <typename T>
inline void fi(T *a)
{
    register char c=0;
    while (c<33) c=getchar_unlocked();
    *a=0;
    int tmp = 0;
    while (c>33)
    {
        if ( c == 45 ) tmp = 1;
        else *a=*a*10+c-'0';
        c=getchar_unlocked();
    }
    if ( tmp == 1 ) *a = 0-(*a);
}

void dfs(int curr, int prev)
{
    par[0][curr] = prev;
    for ( int i = 1; i < MAXLVL; i++ ) par[i][curr] = -1;
    for ( int i = 0; i < (int)v[curr].size(); i++ ) {
        if ( v[curr][i].first == prev ) continue;
        depth[v[curr][i].first] = depth[curr] + 1;
        dfs(v[curr][i].first, curr);
        child[curr] += child[v[curr][i].first] + 1;
    }
}

void HLD(int curr, int prev)
{
    if ( chain_head[chain-1] == -1 ) chain_head[chain-1] = curr;
    chain_index[curr] = chain-1;
    chain_pos[curr] = chain_size[chain-1];
    pos_base_array[curr] = base_array_ptr;
    chain_size[chain-1]++;
    base_array_ptr++;


    int tmp = -1;
    for ( int i = 0; i < (int)v[curr].size(); i++ ) {
        if ( v[curr][i].first == prev ) continue;
        if ( tmp == -1 || child[v[curr][i].first] > child[tmp] ) tmp = v[curr][i].first;
    }

    if ( tmp != -1 ) HLD(tmp, curr);

    for ( int i = 0; i < (int)v[curr].size(); i++ ) {
        if ( v[curr][i].first == prev ) continue;
        if ( v[curr][i].first != tmp ) {
            chain++;
            HLD(v[curr][i].first, curr);
        }
        base_array_val[pos_base_array[v[curr][i].first]] = v[curr][i].second;
    }
}

void init()
{
    for ( int i = 0; i < n; i++ ) {
        v[i].clear();
        depth[i] = child[i] = chain_size[i] = 0;
        chain_head[i] = -1;
    }
    base_array_ptr = 0;
    chain = 1;
}

void pre()
{
    for ( int j = 1; j < MAXLVL; j++ ) {
        for ( int i = 0; i < n; i++ ) {
            if ( par[j-1][i] == -1 ) continue;
            par[j][i] = par[j-1][par[j-1][i]];
        }
    }
    return;
}

int LCA(int x, int y)
{
    if ( depth[x] < depth[y] ) swap(x,y);
    int diff = depth[x] - depth[y];
    for ( int i = 0; i < MAXLVL; i++ ) {
        if ( diff&(1<<i) ) x = par[i][x];
    }
    if ( x == y ) return x;
    for ( int i = MAXLVL-1; i >= 0; i-- ) {
        if ( par[i][x] != par[i][y] ) {
            x = par[i][x];
            y = par[i][y];
        }
    }
    return par[0][x];
}

void build(int where, int left, int right)
{
    if ( left > right ) return;
    if ( left == right ) {
        tree[where] = base_array_val[left];
        return;
    }
    int mid = (left+right)/2;
    build(where*2, left, mid);
    build(where*2+1, mid+1, right);
    tree[where] = tree[where*2] + tree[where*2+1];
}

void update(int where, int left, int right, int idx, int val)
{
    if ( left > right || left > idx || right < idx ) return;
    if ( left == right ) {
        tree[where] = val;
        return;
    }
    int mid = (left+right)/2;
    update(where*2, left, mid, idx, val);
    update(where*2+1, mid+1, right, idx, val);
    tree[where] = tree[where*2] + tree[where*2+1];
}

int query(int where, int left, int right, int i, int j)
{
    if ( left > right || left > j || right < i ) return 0;
    if ( left >= i && right <= j ) return tree[where];
    int mid = (left+right)/2;
    return query(where*2, left, mid, i, j) + query(where*2+1, mid+1, right, i, j);
}

int query(int x, int y)
{
    if ( x == y ) return 0;
    int x_chain;
    int y_chain = chain_index[y];
    int ans = 0;
    while ( 1 ) {
        x_chain = chain_index[x];
        if ( x_chain == y_chain ) {
            ans += query(1,0,n-1, pos_base_array[y]+1, pos_base_array[x]);
            break;
        }
        ans += query(1,0,n-1, pos_base_array[chain_head[x_chain]], pos_base_array[x]);
        x = chain_head[x_chain];
        x = par[0][x];
    }
    return ans;
}
int main()
{
    int q,x,y,z,type,id;

    fi(&n);

    init();

    for ( int i = 0; i < n-1; i++ ) {
        fi(&x), fi(&y), fi(&z);
        x--, y--;
        v[x].push_back(make_pair(y,z));
        v[y].push_back(make_pair(x,z));
        edges[i] = make_pair(x,y);
    }

    dfs(0,-1);
    pre();
    HLD(0,-1);
    build(1,0,n-1);

    fi(&q);

    while ( q-- ) {
        fi(&type);
        if ( type == 1 ) {
            fi(&id), fi(&z);
            id--;
            int fs = edges[id].first;
            int sc = edges[id].second;
            if ( depth[fs] > depth[sc] ) update(1,0,n-1,pos_base_array[fs], z);
            else update(1,0,n-1,pos_base_array[sc], z);
        }
        else {
            fi(&x), fi(&y);
            x--, y--;
            int lca = LCA(x,y);
            int ans = query(x,lca) + query(y,lca);
            printf("%d\n", ans);
        }
    }
    return 0;
}
