#include <iostream>
#include <vector>

using namespace std;

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n;
    cin >> n;

    vector<int> dp(n + 1, 0);
    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i - 1] + 1;

        if (i % 2 == 0) {
            dp[i] = min(dp[i], dp[i / 2] + 1);
        }

        if (i % 3 == 0) {
            dp[i] = min(dp[i], dp[i / 3] + 1);
        }
    }

    cout << dp[n] << endl;
    vector<int> result = {n};
    while (n > 1) {
        if (n % 3 == 0 && dp[n] == dp[n / 3] + 1) {
            n /= 3;
        } else if (n % 2 == 0 && dp[n] == dp[n / 2] + 1) {
            n /= 2;
        } else {
            n--;
        }

        result.push_back(n);
    }

    for (int i = result.size() - 1; i >= 0; i--) {
        cout << result[i] << " ";
    }
    cout << endl;
}
