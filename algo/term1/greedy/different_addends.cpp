#include <iostream>
#include <vector>

using namespace std;

// Taking smallest numbers until the adding of the next number would exceed n
// Then we increase the last number to make the sum equal to n
int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n;
    cin >> n;

    int sum = 0, current_number = 1;
    vector<int> numbers;

    while (true) {
        if (sum + current_number > n) {
            numbers.back() += n - sum;
            break;
        }

        numbers.push_back(current_number);
        sum += current_number;
        ++current_number;
    }

    cout << numbers.size() << endl;
    for (int i = 0; i < numbers.size(); ++i) {
        cout << numbers[i] << " ";
    }
}
