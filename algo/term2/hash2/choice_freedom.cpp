#include <iostream>
#include <set>
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

        for (int i = 1; i <= n; i++) {
            p1[i] = (p1[i - 1] * P1) % MOD1;
            h1[i] = (h1[i - 1] * P1 + (s[i - 1] - 'a' + 1)) % MOD1;

            p2[i] = (p2[i - 1] * P2) % MOD2;
            h2[i] = (h2[i - 1] * P2 + (s[i - 1] - 'a' + 1)) % MOD2;
        }
    }

    pair<ll, ll> get_hash(int l, int r) {
        ll hash_value1 = (h1[r] - h1[l - 1] * p1[r - l + 1] % MOD1 + MOD1) % MOD1;
        ll hash_value2 = (h2[r] - h2[l - 1] * p2[r - l + 1] % MOD2 + MOD2) % MOD2;
        return {hash_value1, hash_value2};
    }
};

void solve() {
    int n;
    cin >> n;

    string a, b;
    cin >> a >> b;

    Hasher hasher_a(a);
    Hasher hasher_b(b);

    int l = 0, r = n + 1;
    pair<ll, ll> pos;
    while (r - l > 1) {
        int m = (l + r) / 2;

        set<pair<ll, ll>> fh;
        for (int i = 0; i + m - 1 < n; i++) {
            fh.insert(hasher_a.get_hash(i + 1, i + m));
        }

        bool found = false;
        for (int i = 0; i + m - 1 < n; i++) {
            if (fh.count(hasher_b.get_hash(i + 1, i + m))) {
                found = true;
                pos = {i + 1, i + m};
                break;
            }
        }

        if (found) {
            l = m;
        } else {
            r = m;
        }
    }

    if (l > 0) {
        cout << b.substr(pos.first - 1, l) << endl;
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

