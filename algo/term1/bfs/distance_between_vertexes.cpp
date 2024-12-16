#include <iostream>
#include <queue>
#include <set>
#include <vector>

using namespace std;

int djikstra(const vector<set<pair<int, int>>> &g, int start, int finish) {
    vector<int> d(g.size(), INT_MAX);
    d[start] = 0;

    priority_queue<pair<int, int>> q;
    q.push({0, start});

    while (!q.empty()) {
        int v = q.top().second;
        int d_v = -q.top().first;
        q.pop();

        if (d_v > d[v]) {
            continue;
        }

        for (auto [u, w] : g[v]) {
            if (d[v] + w < d[u]) {
                d[u] = d[v] + w;
                q.push({-d[u], u});
            }
        }
    }

    return d[finish];
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;

    int s, t;
    cin >> s >> t;
    s--, t--;

    vector<set<pair<int, int>>> g(n);
    for (int i = 0; i < m; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        u--, v--;

        g[u].insert({v, w});
        g[v].insert({u, w});
    }

    int d = djikstra(g, s, t);
    cout << (d == INT_MAX ? -1 : d) << '\n';
}
