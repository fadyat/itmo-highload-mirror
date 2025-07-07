#include <iostream>
#include <set>

using namespace std;

void solve() {
    set<int> bst;
    string command;

    while (cin >> command) {
        int x;
        cin >> x;

        if (command == "insert") {
            bst.insert(x);
        } else if (command == "delete") {
            bst.erase(x);
        } else if (command == "exists") {
            cout << (bst.find(x) != bst.end() ? "true" : "false") << endl;
        } else if (command == "next") {
            auto it = bst.upper_bound(x);
            cout << (it == bst.end() ? "none" : to_string(*it)) << endl;
        } else if (command == "prev") {
            auto it = bst.lower_bound(x);
            cout << (it == bst.begin() ? "none" : to_string(*prev(it))) << endl;
        }
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

