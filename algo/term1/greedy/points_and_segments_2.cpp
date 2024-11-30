#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

enum EventType { SEGMENT_START, POINT, SEGMENT_END };

struct Event {
    int idx;
    EventType type;
    int value;
};

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;

    vector<Event> events;

    for (int i = 0; i < n; ++i) {
        int l, r;
        cin >> l >> r;

        events.push_back({i, SEGMENT_START, min(l, r)});
        events.push_back({i, SEGMENT_END, max(l, r)});
    }

    for (int i = 0; i < m; ++i) {
        int point;
        cin >> point;
        events.push_back({i, POINT, point});
    }

    sort(events.begin(), events.end(), [](const Event &a, const Event &b) {
        if (a.value == b.value) {
            return a.type < b.type;
        }

        return a.value < b.value;
    });

    int active_segments = 0;
    vector<int> result(m, 0);

    for (const Event &e : events) {
        if (e.type == SEGMENT_START) {
            ++active_segments;
        } else if (e.type == SEGMENT_END) {
            --active_segments;
        } else {
            result[e.idx] = active_segments;
        }
    }

    for (int i = 0; i < m; ++i) {
        cout << result[i] << " ";
    }
}
