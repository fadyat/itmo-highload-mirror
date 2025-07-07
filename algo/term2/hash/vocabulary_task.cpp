#include <iostream>
#include <unordered_map>

using namespace std;

void solve() {
    int n;
    cin >> n;

    unordered_map<int, int> m;
    for (int i = 1; i <= n; i++) {
        char op;
        int x;
        cin >> op >> x;

        if (op == '+') {
            m[x]++;
        } else if (op == '-') {
            if (m.find(x) == m.end() || m[x] == 0) {
                cout << "query #" << i << ": can not delete " << x << "\n";
            } else {
                m[x]--;
                if (m[x] == 0) {
                    cout << "after query " << i << " number " << x << " disappeared\n";
                }
            }
        }
    }
};

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

