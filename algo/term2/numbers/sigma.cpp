#include <iostream>
#include <vector>

using namespace std;

const int MAX_N = 5 * 1e6 + 100;

void calculate_sum_of_divisors(vector<long long>& sum_div) {
    for (int i = 1; i <= MAX_N; i++) {
        for (int j = i; j <= MAX_N; j += i) {
            sum_div[j] += i;
        }
    }
}

void solve() {
    int l, r;
    vector<long long> sum_div(MAX_N + 1, 0);
    calculate_sum_of_divisors(sum_div);

    vector<long long> prefix_sum(MAX_N + 1, 0);
    for (int i = 1; i <= MAX_N; i++) {
        prefix_sum[i] = prefix_sum[i - 1] + sum_div[i];
    }

    while (cin >> l >> r) {
        cout << prefix_sum[r] - prefix_sum[l - 1] << endl;
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

