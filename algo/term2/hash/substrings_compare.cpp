#include <iostream>
#include <vector>

using namespace std;

typedef long long ll;

const ll MOD1 = 1e9 + 7;
const ll MOD2 = 1e9 + 9;
const ll P1 = 31;
const ll P2 = 37;

vector<ll> p1, h1, p2, h2;

void preprocess(const string& s) {
    int n = s.size();
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
    ll hv1 = (h1[r] - h1[l - 1] * p1[r - l + 1] % MOD1 + MOD1) % MOD1;
    ll hv2 = (h2[r] - h2[l - 1] * p2[r - l + 1] % MOD2 + MOD2) % MOD2;
    return {hv1, hv2};
}

void solve() {
    string s;
    cin >> s;
    preprocess(s);

    int q;
    cin >> q;

    while (q--) {
        int a, b, c, d;
        cin >> a >> b >> c >> d;
        cout << (get_hash(a, b) == get_hash(c, d) ? "Yes" : "No") << endl;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    solve();
}

