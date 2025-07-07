#include <climits>
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

struct StringWithHash {
    string s;
    Hasher hasher;

    StringWithHash(const string& s) : s(s), hasher(s) {}
};

void solve() {
    int k;
    cin >> k;

    vector<StringWithHash> strings;
    size_t min_len = INT_MAX;
    for (int i = 0; i < k; i++) {
        string s;
        cin >> s;
        strings.emplace_back(s);
        min_len = min(min_len, s.size());
    }

    if (k == 1) {
        cout << strings[0].s << endl;
        return;
    }

    pair<ll, ll> answer_position;
    size_t l = 0, r = min_len + 1;
    while (r - l > 1) {
        size_t m = (l + r) / 2;

        set<pair<ll, ll>> matching_hashes;
        for (int i = 0; i + m <= strings[0].s.size(); i++) {
            matching_hashes.insert(strings[0].hasher.get_hash(i + 1, i + m));
        }

        pair<ll, ll> pos;
        bool found = true;
        for (int q = 1; q < k; q++) {
            set<pair<ll, ll>> new_matching_hashes;
            for (int i = 0; i + m <= strings[q].s.size(); i++) {
                auto hash = strings[q].hasher.get_hash(i + 1, i + m);
                if (matching_hashes.count(hash)) {
                    new_matching_hashes.insert(hash);
                    pos = {i + 1, i + m};
                }
            }

            if (new_matching_hashes.empty()) {
                found = false;
                break;
            }

            matching_hashes = new_matching_hashes;
        }

        if (found) {
            l = m;
            answer_position = pos;
        } else {
            r = m;
        }
    }

    if (l > 0) {
        cout << strings[k - 1].s.substr(answer_position.first - 1, l) << endl;
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

