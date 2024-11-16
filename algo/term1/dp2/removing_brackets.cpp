#include <iostream>
#include <vector>

using namespace std;

bool is_pair(char a, char b) { return (a == '(' && b == ')') || (a == '[' && b == ']') || (a == '{' && b == '}'); }

void reconstruct(int i, int j, string &result, const string &s, const vector<vector<int>> &dp,
                 const vector<vector<int>> &decision) {
    if (i >= j) {
        return;
    }

    if (decision[i][j] == -1) {
        if (dp[i][j] == dp[i + 1][j - 1] + 2) {
            result.push_back(s[i]);
            reconstruct(i + 1, j - 1, result, s, dp, decision);
            result.push_back(s[j]);
        }
    } else {
        int k = decision[i][j];
        if (dp[i][j] == dp[i][k] + dp[k + 1][j]) {
            reconstruct(i, k, result, s, dp, decision);
            reconstruct(k + 1, j, result, s, dp, decision);
        }
    }
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    string s;
    cin >> s;

    int n = s.size();

    // dp[i][j] - the length of the longest valid bracket sequence in the substring []
    vector<vector<int>> dp(n, vector<int>(n, 0));

    // decision[i][j] - the index of the last symbol in the substring [] that we should remove
    vector<vector<int>> decision(n, vector<int>(n, -1));

    for (int l = 2; l <= n; ++l) {
        for (int i = 0; i + l <= n; i++) {
            int j = i + l - 1;

            // if the current substring is a valid bracket sequence
            // then we can add 2 to the result and check the next substring
            if (is_pair(s[i], s[j])) {
                dp[i][j] = dp[i + 1][j - 1] + 2;
                decision[i][j] = -1;
            }

            // potentially, we can remove some symbol from the string
            // and get a better result
            for (int k = i; k < j; ++k) {
                if (dp[i][j] < dp[i][k] + dp[k + 1][j]) {
                    dp[i][j] = dp[i][k] + dp[k + 1][j];
                    decision[i][j] = k;
                }
            }
        }
    }

    string result;
    reconstruct(0, n - 1, result, s, dp, decision);
    cout << result << endl;
}
