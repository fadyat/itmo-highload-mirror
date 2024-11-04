#include <deque>
#include <iostream>

using namespace std;

const long long MOD = 1073741824;

int main() {
    int n, k;
    cin >> n >> k;

    deque<long long> numbers;
    for (int i = 0; i < n; i++) {
        long long number;
        cin >> number;
        numbers.push_back(number);
    }

    while (k--) {
        long long x = numbers.front();
        long long y = numbers.back();

        if (x < y) {
            numbers.pop_front();
            numbers.push_back((x + y) % MOD);
        } else {
            numbers.pop_back();
            numbers.push_front((y - x + MOD) % MOD);
        }
    }

    for (auto number : numbers) {
        cout << number << " ";
    }
}
