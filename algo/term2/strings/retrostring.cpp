#include <iostream>
#include <vector>

using namespace std;

vector<int> compute_prefix(const string& s) {
    vector<int> pi(s.size(), 0);
    for (int i = 1; i < s.size(); i++) {
        int k = pi[i - 1];
        while (k > 0 && s[k] != s[i]) {
            k = pi[k - 1];
        }

        if (s[k] == s[i]) {
            k++;
        }

        pi[i] = k;
    }

    return pi;
}

vector<int> compute_depth(const vector<int>& pi) {
    int n = pi.size();
    vector<int> depth(n + 1, 0);

    for (int i = 1; i <= n; i++) {
        int k = pi[i - 1];
        if (k == 0) {
            depth[i] = 0;
        } else {
            depth[i] = depth[k] + 1;
        }
    }
    return depth;
}

string find_longest_retrostring(const string& s) {
    vector<int> pi = compute_prefix(s);
    vector<int> depth = compute_depth(pi);

    int max_depth = -1, best_len = 0;
    for (int i = 1; i <= s.size(); i++) {
        if (depth[i] > max_depth) {
            max_depth = depth[i];
            best_len = i;
        }
    }

    return s.substr(0, best_len);
}

void solve() {
    string s;
    cin >> s;
    cout << find_longest_retrostring(s) << endl;
}

signed main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

