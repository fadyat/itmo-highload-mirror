#include <climits>
#include <iostream>
#include <vector>

using namespace std;

// Let's use a bitmask to store information about which cities we have already visited.
//
// dp[mask][i] will be the length of the shortest path that visits all cities in the mask
// and ends in city i.
//
// Then dp[mask][i] = min(dp[mask][i], dp[mask ^ (1 << i)][j] + a[j][i]),
// where j is the last city before i in the path, mask ^ (1 << i) is the mask
// in which city i is not visited, and a[j][i] is the length of the path between cities j and i.
//
// Initial values dp[mask][i] = INF for all mask and i,
// except mask = 1 << i, where dp[mask][i] = 0.
int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n;
    cin >> n;

    vector<vector<int>> a(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> a[i][j];
        }
    }

    vector<vector<int>> dp(1 << n, vector<int>(n, INT_MAX));
    for (int i = 0; i < n; i++) {
        dp[1 << i][i] = 0;
    }

    for (int mask = 1; mask < (1 << n); mask++) {
        for (int i = 0; i < n; i++) {
            if ((mask & (1 << i)) == 0) {
                continue;
            }

            for (int j = 0; j < n; j++) {
                if ((mask & (1 << j)) == 0) {
                    continue;
                }

                dp[mask][i] = min(dp[mask][i], dp[mask ^ (1 << i)][j] + a[j][i]);
            }
        }
    }

    int min_path = INT_MAX;
    int end_city = -1;

    for (int i = 0; i < n; i++) {
        if (dp[(1 << n) - 1][i] < min_path) {
            min_path = dp[(1 << n) - 1][i];
            end_city = i;
        }
    }

    vector<int> path;
    int mask = (1 << n) - 1;

    while (mask != 0) {
        path.push_back(end_city);

        int prev_mask = mask ^ (1 << end_city);
        int next_city = -1;

        for (int j = 0; j < n; j++) {
            if ((prev_mask & (1 << j)) == 0) {
                continue;
            }

            if (dp[prev_mask][j] + a[j][end_city] == dp[mask][end_city]) {
                next_city = j;
            }
        }

        end_city = next_city;
        mask = prev_mask;
    }

    cout << min_path << endl;
    for (int i = n - 1; i >= 0; i--) {
        cout << path[i] + 1 << " ";
    }
}
