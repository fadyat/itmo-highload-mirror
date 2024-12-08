#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

int dfs(int current, int started_vertex, int depth, const vector<unordered_set<int>> &g) {
    if (depth == 0) {
        return g[current].count(started_vertex);
    }

    int res = 0;
    for (int next : g[current]) {
        if (next == started_vertex) {
            continue;
        }

        res += dfs(next, started_vertex, depth - 1, g);
    }

    return res;
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;

    vector<unordered_set<int>> g(n);

    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--, v--;

        g[u].insert(v);
        g[v].insert(u);
    }

    int ans = 0;
    for (int i = 0; i < n; i++) {
        ans += dfs(i, i, 2, g);
    }

    cout << ans / 6 << '\n';
}
