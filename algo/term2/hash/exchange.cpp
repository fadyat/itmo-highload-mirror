#include <iostream>
#include <unordered_map>

using namespace std;

void solve() {
    unordered_map<int, int> pos;

    int q;
    cin >> q;

    while (q--) {
        int a, b;
        cin >> a >> b;

        int pos_a = pos.count(a) ? pos[a] : a;
        int pos_b = pos.count(b) ? pos[b] : b;

        pos[a] = pos_b;
        pos[b] = pos_a;

        cout << abs(pos_a - pos_b) << endl;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    solve();
}

