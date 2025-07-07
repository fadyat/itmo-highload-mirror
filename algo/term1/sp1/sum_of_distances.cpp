#include <iostream>
#include <numeric>
#include <queue>
#include <unordered_set>
#include <vector>

using namespace std;

int bfs(const vector<unordered_set<int>> &g, int start) {
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

    return accumulate(d.begin(), d.end(), 0);
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;

    vector<unordered_set<int>> g(n);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        u--, v--;
        g[u].insert(v);
        g[v].insert(u);
    }

    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += bfs(g, i);
    }

    cout << sum / 2 << '\n';
}

