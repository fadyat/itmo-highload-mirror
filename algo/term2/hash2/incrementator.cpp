#include <iostream>
#include <unordered_map>

using namespace std;

void solve() {
    string key;
    int value;
    unordered_map<string, int> vars;

    while (cin >> key >> value) {
        vars[key] += value;
        cout << vars[key] << endl;
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

