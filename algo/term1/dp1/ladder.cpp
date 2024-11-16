#include <iostream>
#include <vector>

using namespace std;

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n;
    cin >> n;

    vector<int> w(n + 1);
    for (int i = 1; i <= n; ++i) {
        cin >> w[i];
    }

    vector<int> dp(n + 1);
    dp[0] = 0;
    dp[1] = w[1];

    for (int i = 2; i <= n; ++i) {
        dp[i] = max(dp[i - 1], dp[i - 2]) + w[i];
    }

    cout << dp[n] << endl;
}
