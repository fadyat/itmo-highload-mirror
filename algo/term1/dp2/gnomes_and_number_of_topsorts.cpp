#include <iostream>
#include <vector>

using namespace std;

#define int long long

// Count the number of topological sorts of the given directed acyclic graph
// with n vertices and m edges.
int count_number_of_topological_sorts(const vector<vector<bool>> &g, int n) {
    vector<int> dp(1 << n, 0);
    dp[0] = 1;

    for (int mask = 0; mask < (1 << n); mask++) {
        for (int i = 0; i < n; i++) {
            // dwarf i'th is already in the mask, skip him
            if ((mask & (1 << i)) != 0) {
                continue;
            }

            // checking, that dwarf i'th matches all the conditions with
            // already added dwarfs, otherwise we can't add him to the mask
            bool ok = true;
            for (int j = 0; j < n; j++) {
                if ((mask & (1 << j)) && g[j][i]) {
                    ok = false;
                    break;
                }
            }

            if (ok) {
                dp[mask | (1 << i)] += dp[mask];
            }
        }
    }

    return dp[(1 << n) - 1];
}

signed main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;

    vector<vector<bool>> g(n, vector<bool>(n, false));
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        a--, b--;
        g[a][b] = true;
    }

    cout << count_number_of_topological_sorts(g, n) << endl;
}
