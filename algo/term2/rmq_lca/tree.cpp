#include <iostream>
#include <queue>
#include <vector>

using namespace std;

const int MAX_LOG = 20;

struct edge {
    int to, w;
};

vector<vector<edge>> graph;
vector<int> depth;
vector<vector<int>> up;
vector<long long> dist;

void dfs() {}

void preprocess(int n) {
    up.resize(MAX_LOG, vector<int>(n, -1));
    depth.resize(n, 0);
    dist.resize(n, 0);

    queue<int> q;
    q.push(0);
    up[0][0] = -1;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (auto e : graph[u]) {
            int v = e.to;
            if (up[0][v] == -1 && up[0][u] != v) {
                up[0][v] = u;
                depth[v] = depth[u] + 1;
                dist[v] = dist[u] + e.w;
                q.push(v);
            }
        }
    }

    for (int k = 1; k < MAX_LOG; ++k) {
        for (int v = 0; v < n; ++v) {
            if (up[k - 1][v] != -1) {
                up[k][v] = up[k - 1][up[k - 1][v]];
            }
        }
    }
}

int lca(int u, int v) {
    if (depth[u] < depth[v]) {
        swap(u, v);
    }

    for (int k = MAX_LOG - 1; k >= 0; --k) {
        if (depth[u] - (1 << k) >= depth[v]) {
            u = up[k][u];
        }
    }

    if (u == v) {
        return u;
    }

    for (int k = MAX_LOG - 1; k >= 0; --k) {
        if (up[k][u] != -1 && up[k][u] != up[k][v]) {
            u = up[k][u];
            v = up[k][v];
        }
    }

    return up[0][u];
}

long long get_distance(int u, int v) {
    int ancestor = lca(u, v);
    return dist[u] + dist[v] - 2 * dist[ancestor];
}

void solve() {
    int n;
    cin >> n;

    graph.resize(n);
    for (int i = 0; i < n - 1; i++) {
        int u, v, w;
        cin >> u >> v >> w;

        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }

    preprocess(n);

    int q;
    cin >> q;
    while (q--) {
        int u, v;
        cin >> u >> v;
        cout << get_distance(u, v) << endl;
    }
}

signed main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

