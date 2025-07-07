#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;

void actual_main() {
    static const long long INF = static_cast<long long>(1e18);

    int n, m, s, t;
    cin >> n >> m >> s >> t;
    s--, t--;

    vector<unordered_map<int, long long>> g(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        long long w;
        cin >> u >> v >> w;
        u--, v--;
        g[u][v] = w;
    }

    vector<long long> d(n, INF);
    d[s] = 0;

    vector<bool> in_queue(n);
    queue<int> q;
    q.push(s);
    in_queue[s] = true;

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        in_queue[u] = false;

        for (const auto& [v, w] : g[u]) {
            if (d[v] > d[u] + w) {
                d[v] = d[u] + w;
                if (!in_queue[v]) {
                    q.push(v);
                    in_queue[v] = true;
                }
            }
        }
    }

    cout << d[t] << endl;
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    actual_main();
}

