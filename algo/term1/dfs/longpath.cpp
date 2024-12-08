#include <iostream>
#include <stack>
#include <vector>

using namespace std;

void top_sort(int v, vector<vector<int>> &g, vector<bool> &used, stack<int> &top_order) {
    used[v] = true;

    for (int u : g[v]) {
        if (!used[u]) {
            top_sort(u, g, used, top_order);
        }
    }

    top_order.push(v);
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

    vector<bool> used(n, false);
    stack<int> top_order;

    for (int i = 0; i < n; i++) {
        if (!used[i]) {
            top_sort(i, g, used, top_order);
        }
    }

    vector<int> dist(n, 0);
    while (!top_order.empty()) {
        int v = top_order.top();
        top_order.pop();

        for (int u : g[v]) {
            dist[u] = max(dist[u], dist[v] + 1);
        }
    }

    cout << *max_element(dist.begin(), dist.end()) << endl;
}
