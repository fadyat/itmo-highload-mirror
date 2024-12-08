#include <iostream>
#include <vector>

using namespace std;

bool dfs(int v, int t, const vector<vector<int>> &g, vector<bool> &used, vector<int> &path) {
    used[v] = true;
    path.push_back(v);

    if (v == t) {
        return true;
    }

    for (int u : g[v]) {
        if (!used[u]) {
            if (dfs(u, t, g, used, path)) {
                return true;
            }
        }
    }

    path.pop_back();
    return false;
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, m, s, t;
    cin >> n >> m >> s >> t;
    s--, t--;

    vector<vector<int>> g(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--, v--;

        g[u].push_back(v);
    }

    vector<bool> used(n, false);
    vector<int> path;

    if (dfs(s, t, g, used, path)) {
        for (int v : path) {
            cout << v + 1 << ' ';
        }
    } else {
        cout << -1;
    }
}
