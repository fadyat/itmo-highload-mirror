#include <iostream>
#include <vector>

using namespace std;

void solve() {
    int n;
    cin >> n;

    vector<int> zfunc(n);
    for (int i = 0; i < n; i++) {
        cin >> zfunc[i];
    }

    vector<int> pfunc(n);
    for (int i = 1; i < n; i++) {
        for (int j = zfunc[i] - 1; j >= 0; j--) {
            if (pfunc[i + j] > 0) {
                break;
            }

            pfunc[i + j] = j + 1;
        }
    }

    for (int i = 0; i < n; i++) {
        cout << pfunc[i] << " ";
    }
}

signed main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

