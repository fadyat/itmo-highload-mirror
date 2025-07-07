#include <iostream>
#include <vector>

using namespace std;

vector<int> get_primes(int n) {
    vector<bool> is_prime(n + 1, true);
    is_prime[0] = false;
    is_prime[1] = false;

    for (int i = 2; i * i <= n; i++) {
        if (is_prime[i]) {
            for (int j = i * i; j <= n; j += i) {
                is_prime[j] = false;
            }
        }
    }

    vector<int> primes;
    for (int i = 0; i < is_prime.size(); i++) {
        if (is_prime[i]) {
            primes.push_back(i);
        }
    }

    return primes;
}

void solve() {
    int q;
    cin >> q;

    vector<int> primes = get_primes(1e7);
    while (q--) {
        int k;
        cin >> k;
        cout << primes[k - 1] << endl;
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

