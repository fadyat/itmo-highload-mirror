#include <iostream>
#include <vector>

using namespace std;

struct edge {
    int u, v;
    long long w;
};

void dfs(int s, const vector<edge>& edges, vector<int>& d, vector<bool>& visited) {
    visited[s] = true;
    for (const auto& e : edges) {
        if (e.u == s && !visited[e.v]) {
            dfs(e.v, edges, d, visited);
        }
    }
}

void actual_main() {
    int n, m;
    cin >> n >> m;

    vector<edge> edges;
    for (int i = 0; i < m; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        u--, v--;
        edges.push_back({u, v, w});
    }

    vector<int> d(n, -INT_MAX);
    d[0] = 0;

    int k = 0;
    bool run = true;

    while (run && k < n) {
        run = false;
        for (const auto& e : edges) {
            if (d[e.u] > -INT_MAX && d[e.v] < d[e.u] + e.w) {
                d[e.v] = d[e.u] + e.w;
                run = true;
            }
        }

        k++;
    }

    vector<bool> visited(n);
    for (const auto& e : edges) {
        if (d[e.u] > -INT_MAX && d[e.v] < d[e.u] + e.w) {
            dfs(e.u, edges, d, visited);
        }
    }

    if (d[n - 1] == -INT_MAX) {
        cout << ":(" << endl;
    } else if (visited[n - 1]) {
        cout << ":)" << endl;
    } else {
        cout << d[n - 1] << endl;
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    actual_main();
}

