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
    string a, b;
    cin >> a >> b;
    int n = a.size(), m = b.size();

    Hasher ha(a);
    Hasher hb(b);

    auto hash_b = hb.get_hash(1, m);

    vector<int> positions;
    for (int i = 1; i + m - 1 <= n; i++) {
        if (ha.get_hash(i, i + m - 1) == hash_b) {
            positions.push_back(i);
        }
    }

    cout << positions.size() << endl;
    for (int i = 0; i < positions.size(); i++) {
        cout << positions[i] << " ";
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    solve();
}

