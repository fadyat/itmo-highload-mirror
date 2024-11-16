#include <iostream>
#include <map>
#include <vector>

using namespace std;

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n;
    cin >> n;

    vector<int> a(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }

    map<pair<int, int>, int> dp;
    for (int i = 0; i < n; i++) {
        dp[{i, a[i]}] = 1;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (a[i] % a[j] == 0) {
                dp[{i, a[i]}] = max(dp[{i, a[i]}], dp[{j, a[j]}] + 1);
            }
        }
    }

    int ans = 0;
    for (auto &[_, value] : dp) {
        ans = max(ans, value);
    }

    cout << ans << endl;
}
