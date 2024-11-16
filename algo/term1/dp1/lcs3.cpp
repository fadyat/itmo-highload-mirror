#include <iostream>
#include <vector>

using namespace std;

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, m, k;
    vector<int> a, b, c;

    cin >> n;
    a.resize(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }

    cin >> m;
    b.resize(m);
    for (int i = 0; i < m; i++) {
        cin >> b[i];
    }

    cin >> k;
    c.resize(k);
    for (int i = 0; i < k; i++) {
        cin >> c[i];
    }

    vector<vector<vector<int>>> dp(n + 1, vector<vector<int>>(m + 1, vector<int>(k + 1, 0)));
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            for (int l = 1; l <= k; l++) {
                if (a[i - 1] == b[j - 1] && a[i - 1] == c[l - 1]) {
                    dp[i][j][l] = dp[i - 1][j - 1][l - 1] + 1;
                } else {
                    dp[i][j][l] = max(dp[i - 1][j][l], max(dp[i][j - 1][l], dp[i][j][l - 1]));
                }
            }
        }
    }

    cout << dp[n][m][k] << endl;
}
