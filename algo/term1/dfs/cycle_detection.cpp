#include <iostream>
#include <vector>

using namespace std;

bool dfs(int v, vector<vector<int>> &g, vector<int> &visited, vector<int> &path) {
    visited[v] = 2;
    path.push_back(v);

    for (int u : g[v]) {
        if (visited[u] == 0) {
            if (dfs(u, g, visited, path)) {
                return true;
            }
        } else if (visited[u] == 2) {
            // cycle detected
            path.push_back(u);
            return true;
        }
    }

    visited[v] = 1;
    path.pop_back();
    return false;
}

void show_path(vector<int> &path) {
    cout << "YES" << endl;

    int cycle_start = path.back();
    path.pop_back();
    vector<int> cycle;
    while (path.back() != cycle_start) {
        cycle.push_back(path.back());
        path.pop_back();
    }

    cycle.push_back(cycle_start);
    for (auto it = cycle.rbegin(); it != cycle.rend(); it++) {
        cout << *it + 1 << ' ';
    }
    cout << endl;
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;

    vector<vector<int>> g(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--, v--;

        g[u].push_back(v);
    }

    // 0 - not visited, 1 - visited, 2 - visited and in stack
    // if we find a vertex that is in stack, then we have a cycle
    vector<int> visited(n, 0);
    vector<int> path;

    for (int i = 0; i < n; i++) {
        if (visited[i] == 0) {
            if (dfs(i, g, visited, path)) {
                show_path(path);
                return 0;
            }
        }
    }

    cout << "NO" << endl;
}
