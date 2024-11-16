#include <iostream>
#include <vector>

using namespace std;

vector<vector<int>> g;
vector<bool> win;
vector<bool> visited;

// - if node is a leaf, then it is a losing position
// - if node is not a leaf, then if we have some child that is a losing position,
//   then this node is a winning position
// - if all children are winning positions, then this node is a losing position
//
// Let's calculate the answer for each node in a bottom-up manner
// If the answer for the root is 1, then Genry wins, otherwise he loses
void dfs(int node) {
    visited[node] = true;

    if (g[node].empty()) {
        win[node] = false;
    } else {
        for (int child : g[node]) {
            if (!visited[child]) {
                dfs(child);
            }

            if (!win[child]) {
                win[node] = true;
            }
        }
    }
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n;
    cin >> n;

    int root_idx = 0;
    vector<int> p(n + 1);
    for (int i = 1; i <= n; i++) {
        cin >> p[i];

        if (p[i] == 0) {
            root_idx = i;
        }
    }

    g.assign(n + 1, vector<int>());
    for (int i = 1; i <= n; i++) {
        if (p[i] != 0) {
            g[p[i]].push_back(i);
        }
    }

    win.assign(n + 1, false);
    visited.assign(n + 1, false);

    dfs(root_idx);
    cout << (win[root_idx] ? "YES" : "NO") << endl;
}
