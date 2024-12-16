#include <iostream>
#include <queue>
#include <vector>

using namespace std;

vector<int> bfs(const vector<vector<int>> &g, int start) {
    vector<int> d(g.size(), -1);
    d[start] = 0;

    queue<int> q;
    q.push(start);

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        for (int u : g[v]) {
            if (d[u] == -1) {
                d[u] = d[v] + 1;
                q.push(u);
            }
        }
    }

    return d;
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, s, m;
    cin >> n >> s >> m;
    s--;

    vector<vector<int>> g(n);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        u--, v--;

        // inverting the path to make count
        // of edges from the capital to the city
        g[v].push_back(u);
    }

    vector<int> d = bfs(g, s);
    for (int i = 0; i < n; ++i) {
        cout << d[i] << ' ';
    }
}
