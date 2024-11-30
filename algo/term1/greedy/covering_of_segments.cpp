#include <iostream>
#include <vector>

using namespace std;

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n;
    cin >> n;

    vector<pair<int, int>> segments(n);
    for (int i = 0; i < n; i++) {
        cin >> segments[i].first >> segments[i].second;
    }

    sort(segments.begin(), segments.end(),
         [](const pair<int, int> &a, const pair<int, int> &b) { return a.second < b.second; });

    vector<int> points;
    for (int i = 0; i < n; i++) {
        if (points.empty() || points.back() < segments[i].first) {
            points.push_back(segments[i].second);
        }
    }

    cout << points.size() << endl;
    for (int point : points) {
        cout << point << " ";
    }
}
