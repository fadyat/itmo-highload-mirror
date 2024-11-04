#include <algorithm>
#include <iostream>
#include <optional>
#include <vector>

using namespace std;

optional<vector<long long>> count_intersection(vector<long long> &a, vector<long long> &b) {
    vector<long long> intersection = {// left bottom corner
                                      max(a[0], b[0]), max(a[1], b[1]),

                                      // right top corner
                                      min(a[2], b[2]), min(a[3], b[3])};

    long long width = intersection[2] - intersection[0];
    long long height = intersection[3] - intersection[1];

    if (width < 0 || height < 0) {
        return std::nullopt;
    }

    return intersection;
}

int main() {
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);

    int n;
    cin >> n;

    vector<long long> intersection = {0, 0, 0, 0};
    cin >> intersection[0] >> intersection[1] >> intersection[2] >> intersection[3];
    n--;

    while (n--) {
        vector<long long> rectangle = {0, 0, 0, 0};
        cin >> rectangle[0] >> rectangle[1] >> rectangle[2] >> rectangle[3];

        optional<vector<long long>> opt = count_intersection(intersection, rectangle);
        if (!opt.has_value()) {
            intersection = {0, 0, 0, 0};
            break;
        }

        intersection = opt.value();
    }

    long long width = intersection[2] - intersection[0];
    long long height = intersection[3] - intersection[1];
    cout << max(width, 0LL) * max(height, 0LL) << endl;
}
