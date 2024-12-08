#include <iostream>
#include <stack>
#include <vector>

using namespace std;

bool _dfs(int v, vector<vector<int>> &g, vector<int> &visited) {
    visited[v] = 2;

    for (int u : g[v]) {
        if (visited[u] == 0) {
            if (_dfs(u, g, visited)) {
                return true;
            }
        } else if (visited[u] == 2) {
            return true;
        }
    }

    visited[v] = 1;
    return false;
}

bool check_cycle(int n, vector<vector<int>> &g) {
    vector<int> visited(n, 0);

    for (int i = 0; i < n; i++) {
        if (visited[i] == 0) {
            if (_dfs(i, g, visited)) {
                return true;
            }
        }
    }

    return false;
}

void _top_sort(int v, vector<vector<int>> &g, vector<bool> &used, stack<int> &top_order) {
    used[v] = true;

    for (int u : g[v]) {
        if (!used[u]) {
            _top_sort(u, g, used, top_order);
        }
    }

    top_order.push(v);
}

void top_sort(int n, vector<vector<int>> &g) {
    vector<bool> used(n, false);
    stack<int> top_order;

    for (int i = 0; i < n; i++) {
        if (!used[i]) {
            _top_sort(i, g, used, top_order);
        }
    }

    while (!top_order.empty()) {
        cout << top_order.top() + 1 << ' ';
        top_order.pop();
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
    }

    if (check_cycle(n, g)) {
        cout << -1 << endl;
        return 0;
    }

    top_sort(n, g);
}
