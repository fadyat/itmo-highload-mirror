#include <iostream>
#include <vector>

using namespace std;

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;

    vector<int> points(n);
    for (int i = 0; i < n; i++) {
        cin >> points[i];
    }
    sort(points.begin(), points.end());

    vector<pair<int, int>> segments(m);
    for (int i = 0; i < m; i++) {
        cin >> segments[i].first >> segments[i].second;
    }

    vector<int> result(m);
    for (int i = 0; i < m; i++) {
        int l = segments[i].first;
        int r = segments[i].second;

        auto lower = lower_bound(points.begin(), points.end(), l);
        auto upper = upper_bound(points.begin(), points.end(), r);

        result[i] = upper - lower;
    }

    for (int i = 0; i < m; i++) {
        cout << result[i] << " ";
    }
}
