#include <iostream>
#include <vector>

using namespace std;

struct edge {
    int u, v;
    long long w;
};

static const long long INF = static_cast<long long>(1e20);

void actual_main() {
    int n, m, s;
    cin >> n >> m >> s;
    s--;

    vector<edge> edges;
    for (int i = 0; i < m; i++) {
        int u, v;
        long long w;
        cin >> u >> v >> w;
        u--, v--;
        edges.push_back({u, v, w});
    }

    vector<bool> is_negative_cycle(n);
    vector<long long> d(n, INF);
    d[s] = 0;

    int k = 0;
    bool run = true;

    while (run && k < n) {
        run = false;
        for (const auto& e : edges) {
            if (d[e.u] < INF && d[e.v] > d[e.u] + e.w) {
                d[e.v] = max(-INF, d[e.u] + e.w);
                run = true;
                if (k == n - 1) {
                    is_negative_cycle[e.v] = true;
                }
            }
        }

        k++;
    }

    for (int i = 0; i < n; i++) {
        for (const auto& e : edges) {
            if (is_negative_cycle[e.u]) {
                is_negative_cycle[e.v] = true;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        if (d[i] == INF) {
            cout << '*' << endl;
        } else if (is_negative_cycle[i]) {
            cout << '-' << endl;
        } else {
            cout << d[i] << endl;
        }
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    actual_main();
}

