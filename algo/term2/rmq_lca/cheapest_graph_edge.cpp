#include <iostream>
#include <vector>

using namespace std;

const int MAX_LOG = 20;

struct edge {
    int to, w;
};

struct node {
    int a;
    int min_edge;
};

vector<edge> parent;
vector<int> depth;
vector<vector<node>> up;
vector<long long> dist;

void preprocess(int n) {
    up.resize(MAX_LOG, vector<node>(n, {-1, INT_MAX}));
    depth.resize(n, 0);
    dist.resize(n, 0);

    for (int i = 1; i < n; ++i) {
        int v = parent[i].to;
        up[0][i].a = v;
        up[0][i].min_edge = parent[i].w;
        depth[i] = depth[v] + 1;
    }

    for (int k = 1; k < MAX_LOG; ++k) {
        for (int v = 0; v < n; ++v) {
            if (up[k - 1][v].a != -1) {
                int ancestor = up[k - 1][v].a;
                up[k][v].a = up[k - 1][ancestor].a;
                up[k][v].min_edge = min(up[k - 1][v].min_edge, up[k - 1][ancestor].min_edge);
            }
        }
    }
}

int lca(int u, int v) {
    int min_val = INT_MAX;

    if (depth[u] < depth[v]) {
        swap(u, v);
    }

    for (int k = MAX_LOG - 1; k >= 0; --k) {
        if (depth[u] - (1 << k) >= depth[v]) {
            min_val = min(min_val, up[k][u].min_edge);
            u = up[k][u].a;
        }
    }

    if (u == v) {
        return min_val;
    }

    for (int k = MAX_LOG - 1; k >= 0; --k) {
        if (up[k][u].a != -1 && up[k][u].a != up[k][v].a) {
            min_val = min(min_val, min(up[k][u].min_edge, up[k][v].min_edge));
            u = up[k][u].a;
            v = up[k][v].a;
        }
    }

    min_val = min(min_val, min(up[0][u].min_edge, up[0][v].min_edge));
    return min_val;
}

void solve() {
    int n;
    cin >> n;

    parent.resize(n);
    for (int i = 1; i < n; ++i) {
        cin >> parent[i].to >> parent[i].w;
        parent[i].to--;
    }

    preprocess(n);

    int q;
    cin >> q;
    while (q--) {
        int u, v;
        cin >> u >> v;
        cout << lca(u - 1, v - 1) << endl;
    }
}

signed main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

