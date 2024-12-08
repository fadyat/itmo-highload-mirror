#include <iostream>
#include <vector>

using namespace std;

void dfs(int current, int component_id, vector<int> &component, const vector<vector<int>> &g) {
    component[current] = component_id;

    for (int next : g[current]) {
        if (component[next] == -1) {
            dfs(next, component_id, component, g);
        }
    }
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
        g[v].push_back(u);
    }

    vector<int> component(n, -1);
    int components = 0;
    for (int i = 0; i < n; i++) {
        if (component[i] == -1) {
            dfs(i, components++, component, g);
        }
    }

    cout << components << '\n';
    for (int i = 0; i < n; i++) {
        cout << component[i] + 1 << ' ';
    }
}
