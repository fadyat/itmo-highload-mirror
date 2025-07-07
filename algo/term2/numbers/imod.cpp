#include <iostream>

using namespace std;

long long extended_gcd(long long a, long long b, long long& x, long long& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }

    long long x1, y1;
    long long gcd = extended_gcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return gcd;
}

void solve() {
    long long a, m;
    cin >> a >> m;

    long long x, y;
    long long gcd = extended_gcd(a, m, x, y);
    cout << ((gcd != 1) ? (-1) : ((x % m + m) % m));
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

