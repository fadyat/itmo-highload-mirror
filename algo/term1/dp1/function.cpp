#include <cmath>
#include <iostream>
#include <map>

using namespace std;

const long long MOD = 1ULL << 32;

long long fn(long long n, map<long long, long long> &memo) {
    if (n <= 2) {
        return 1;
    }

    if (memo.find(n) != memo.end()) {
        return memo[n];
    }

    if (n % 2 == 1) {
        return memo[n] = (fn(floor(6 * n / 7), memo) + fn(floor(2 * n / 3), memo)) % MOD;
    }

    return memo[n] = (fn(n - 1, memo) + fn(n - 3, memo)) % MOD;
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    long long n;
    cin >> n;

    map<long long, long long> memo;
    cout << fn(n, memo) << endl;
}
