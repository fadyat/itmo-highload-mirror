#include <cstdint>
#include <iostream>
#include <unordered_set>

using namespace std;

void add_point(int32_t x, int32_t y, size_t w, size_t h, unordered_set<int64_t>& canvas) {
    if (x >= 1 && x <= w && y >= 1 && y <= h) {
        canvas.insert(static_cast<int64_t>(x) << 32 | y);
    }
}

void solve() {
    size_t w, h, n;
    cin >> w >> h >> n;

    unordered_set<int64_t> canvas;
    canvas.reserve(5 * n);

    while (n--) {
        int32_t x, y;
        cin >> x >> y;

        add_point(x, y, w, h, canvas);
        add_point(x - 1, y, w, h, canvas);
        add_point(x, y - 1, w, h, canvas);
        add_point(x + 1, y, w, h, canvas);
        add_point(x, y + 1, w, h, canvas);
    }

    cout << (canvas.size() == w * h ? "Yes" : "No") << endl;
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

