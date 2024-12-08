#include <iostream>
#include <vector>

using namespace std;

struct cell {
    int x, y;
};

vector<cell> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

bool can_go(vector<vector<bool>> &g, cell c) {
    return c.x >= 0 && c.x < g.size() && c.y >= 0 && c.y < g[0].size() && g[c.x][c.y];
}

bool _dfs(vector<vector<bool>> &g, cell current, cell finish, vector<vector<bool>> &used, vector<cell> &path) {
    if (current.x == finish.x && current.y == finish.y) {
        path.push_back(current);
        return true;
    }

    used[current.x][current.y] = true;
    for (cell d : directions) {
        cell next = {current.x + d.x, current.y + d.y};
        if (can_go(g, next) && !used[next.x][next.y]) {
            if (_dfs(g, next, finish, used, path)) {
                path.push_back(current);
                return true;
            }
        }
    }

    return false;
}

vector<cell> dfs(vector<vector<bool>> &g, cell start, cell finish) {
    vector<vector<bool>> used(g.size(), vector<bool>(g[0].size(), 0));
    vector<cell> path;

    if (_dfs(g, start, finish, used, path)) {
        reverse(path.begin(), path.end());
        return path;
    } else {
        return {};
    }
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, m;
    cell start, finish;
    cin >> n >> m >> start.x >> start.y >> finish.x >> finish.y;
    start.x--, start.y--, finish.x--, finish.y--;

    swap(start.x, start.y);
    swap(finish.x, finish.y);
    swap(n, m);

    vector<vector<bool>> g(n, vector<bool>(m, 0));
    for (int i = 0; i < n; i++) {
        string s;
        cin >> s;

        for (int j = 0; j < m; j++) {
            g[i][j] = s[j] == '.';
        }
    }

    vector<cell> path = dfs(g, start, finish);
    if (path.empty()) {
        cout << "NO";
    } else {
        cout << "YES" << '\n';
        for (cell c : path) {
            cout << c.y + 1 << ' ' << c.x + 1 << '\n';
        }
    }
}
