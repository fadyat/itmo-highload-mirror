#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

void solve() {
    string s;
    getline(cin, s);

    int n = s.size();
    const char* str = s.c_str();

    vector<int> suff(n);
    for (int i = 0; i < n; i++) {
        suff[i] = i;
    }

    sort(suff.begin(), suff.end(), [&](int i, int j) { return strcmp(str + i, str + j) < 0; });

    for (int i = 0; i < n; i++) {
        cout << suff[i] + 1 << " ";
    }
}

signed main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

