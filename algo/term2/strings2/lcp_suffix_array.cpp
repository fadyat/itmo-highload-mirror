#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

typedef long long ll;

const ll MOD1 = 1e9 + 7;
const ll MOD2 = 1e9 + 9;
const ll P1 = 31;
const ll P2 = 37;

struct Hasher {
    vector<ll> p1, h1, p2, h2;
    int n;

    Hasher(const string& s) {
        n = s.size();
        p1.resize(n + 1, 1);
        h1.resize(n + 1, 0);
        p2.resize(n + 1, 1);
        h2.resize(n + 1, 0);

        for (int i = 0; i < n; i++) {
            p1[i + 1] = (p1[i] * P1) % MOD1;
            h1[i + 1] = (h1[i] * P1 + (s[i] - 'a' + 1)) % MOD1;

            p2[i + 1] = (p2[i] * P2) % MOD2;
            h2[i + 1] = (h2[i] * P2 + (s[i] - 'a' + 1)) % MOD2;
        }
    }

    // zero indexed, inclusive
    pair<ll, ll> get_hash(int l, int r) const {
        ll hash1 = (h1[r + 1] - h1[l] * p1[r - l + 1] % MOD1 + MOD1) % MOD1;
        ll hash2 = (h2[r + 1] - h2[l] * p2[r - l + 1] % MOD2 + MOD2) % MOD2;
        return {hash1, hash2};
    }
};

int get_lcp(int i, int j, const vector<int>& sa, const Hasher& hasher, int n) {
    int a = sa[i], b = sa[j];
    int l = 0, r = n - max(a, b) + 1;

    while (r - l > 1) {
        int m = (l + r) / 2;
        if (hasher.get_hash(a, a + m - 1) == hasher.get_hash(b, b + m - 1)) {
            l = m;
        } else {
            r = m;
        }
    }

    return l;
}

void solve() {
    int n;
    cin >> n;

    string s;
    cin >> s;

    vector<int> suffix_array(n);
    for (int i = 0; i < n; i++) {
        cin >> suffix_array[i];
        suffix_array[i]--;
    }

    Hasher hasher(s);
    for (int i = 0; i < n - 1; i++) {
        cout << get_lcp(i, i + 1, suffix_array, hasher, n) << endl;
    }
}

signed main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

