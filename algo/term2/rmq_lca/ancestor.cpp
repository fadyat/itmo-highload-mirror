#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

vector<int> entry_time;
vector<int> exit_time;

int timer = 0;

void dfs(int v, const vector<unordered_set<int>>& g) {
    entry_time[v] = timer++;
    for (int u : g[v]) {
        dfs(u, g);
    }

    exit_time[v] = timer++;
}

void solve() {
    int n;
    cin >> n;

    int root_id = -1;
    vector<unordered_set<int>> g(n);
    for (int i = 0; i < n; i++) {
        int parent_id;
        cin >> parent_id;
        if (parent_id != 0) {
            g[parent_id - 1].insert(i);
        } else {
            root_id = i;
        }
    }

    entry_time.resize(n);
    exit_time.resize(n);

    dfs(root_id, g);

    int m;
    cin >> m;

    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--, v--;
        bool is_ancestor = entry_time[u] <= entry_time[v] && exit_time[u] >= exit_time[v];
        cout << (is_ancestor ? 1 : 0) << endl;
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

