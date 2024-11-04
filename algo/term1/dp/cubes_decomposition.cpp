#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    vector<int> requests;
    int n;
    while (cin >> n) {
        requests.push_back(n);
    }

    int max_n = *max_element(requests.begin(), requests.end());

    vector<int> cubes;
    for (int i = 1; i * i * i <= max_n; ++i) {
        cubes.push_back(i * i * i);
    }

    vector<int> dp(max_n + 1, 1e9);
    dp[0] = 0;

    for (int i = 1; i <= max_n; ++i) {
        for (int cube : cubes) {
            if (cube > i) {
                break;
            }

            dp[i] = min(dp[i], dp[i - cube] + 1);
        }
    }

    for (int n : requests) {
        cout << dp[n] << endl;
    }
}
