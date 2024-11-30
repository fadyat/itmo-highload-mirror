#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

struct dwarf {
    int id;
    int fall_asleep_time;
    int sleep_time;
};

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n;
    cin >> n;

    vector<dwarf> dwarfs(n);
    for (int i = 0; i < n; i++) {
        cin >> dwarfs[i].fall_asleep_time;
        dwarfs[i].id = i + 1;
    }

    for (int i = 0; i < n; i++) {
        cin >> dwarfs[i].sleep_time;
    }

    sort(dwarfs.begin(), dwarfs.end(), [](const dwarf &a, const dwarf &b) {
        return a.fall_asleep_time + a.sleep_time < b.fall_asleep_time + b.sleep_time;
    });

    int fall_asleep_time_sum = 0;
    for (int i = 0; i < n; i++) {
        if (fall_asleep_time_sum >= dwarfs[i].sleep_time) {
            cout << -1 << endl;
            return 0;
        }

        fall_asleep_time_sum += dwarfs[i].fall_asleep_time;
    }

    for (auto it = dwarfs.rbegin(); it != dwarfs.rend(); it++) {
        cout << it->id << " ";
    }
}
