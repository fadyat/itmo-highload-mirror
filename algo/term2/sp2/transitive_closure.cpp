#include <bitset>
#include <iostream>
#include <vector>

using namespace std;

void actual_main() {
    static const int MAX_N = 1000;

    int n;
    cin >> n;

    vector<bitset<MAX_N>> d(n);
    for (int i = 0; i < n; i++) {
        string s;
        cin >> s;
        for (int j = 0; j < n; j++) {
            d[i][j] = s[j] - '0';
        }
    }

    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            if (d[i][k] == 1) {
                d[i] |= d[k];
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << d[i][j];
        }
        cout << endl;
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    actual_main();
}

