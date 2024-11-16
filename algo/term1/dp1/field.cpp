#include <iostream>
#include <vector>

using namespace std;

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;

    vector<vector<int>> field(m, vector<int>(n));
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            cin >> field[i][j];
        }
    }

    vector<vector<int>> dp(m, vector<int>(n, 0));
    dp[0][0] = 0;
    for (int i = 1; i < m; i++) {
        dp[i][0] = dp[i - 1][0] + abs(field[i][0] - field[i - 1][0]);
    }

    for (int j = 1; j < n; j++) {
        dp[0][j] = dp[0][j - 1] + abs(field[0][j] - field[0][j - 1]);
    }

    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[i][j] = min(dp[i - 1][j] + abs(field[i][j] - field[i - 1][j]),
                           dp[i][j - 1] + abs(field[i][j] - field[i][j - 1]));
        }
    }

    cout << dp[m - 1][n - 1] << endl;
}
