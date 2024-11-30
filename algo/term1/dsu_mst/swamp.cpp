#include <cmath>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

// can_cross_swamp - checks if it is possible to cross the swamp with the given jump distance.
bool can_cross_swamp(double jump_distance, int h, vector<pair<int, int>> &coords) {
    vector<vector<bool>> graph(coords.size() + 2, vector<bool>(coords.size() + 2, false));
    for (int i = 0; i < coords.size(); i++) {
        graph[0][i + 1] = coords[i].second <= jump_distance;
        graph[i + 1][coords.size() + 1] = h - coords[i].second <= jump_distance;
    }

    for (int i = 0; i < coords.size(); i++) {
        for (int j = 0; j < coords.size(); j++) {
            if (i == j) {
                continue;
            }

            double distance =
                sqrt(pow(coords[i].first - coords[j].first, 2) + pow(coords[i].second - coords[j].second, 2));

            graph[i + 1][j + 1] = distance <= jump_distance;
        }
    }

    vector<bool> visited(graph.size(), false);
    queue<int> q;
    q.push(0);
    visited[0] = true;

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        for (int u = 0; u < graph.size(); u++) {
            if (graph[v][u] && !visited[u]) {
                visited[u] = true;
                q.push(u);
            }
        }
    }

    return visited[coords.size() + 1];
}

// get_minimal_jump_distance - returns the minimal distance required to jump over the swamp.
// we are using binary search with bfs to determine the minimal distance.
double get_minimal_jump_distance(int h, vector<pair<int, int>> &coords) {
    double l = 0, r = h + 100;
    double eps = 1e-10;

    while (r - l > eps) {
        double m = (l + r) / 2;
        bool can_cross_jump = can_cross_swamp(m, h, coords);
        if (can_cross_jump) {
            r = m;
        } else {
            l = m;
        }
    }

    return r;
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    int h, n;
    cin >> h >> n;
    vector<pair<int, int>> coords;
    for (int i = 0; i < n; i++) {
        int x, y;
        cin >> x >> y;
        coords.push_back({x, y});
    }

    cout.precision(10);
    cout << get_minimal_jump_distance(h, coords) << endl;
}
