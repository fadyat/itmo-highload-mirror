#include <cstring>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

const int MAXN = 1e5 + 5;
const int INF = 1e9;

struct edge {
    int a, b, cap, flow;
};

vector<edge> e;
vector<int> g[MAXN];
int n, s, t, d[MAXN], ptr[MAXN];

void add_edge(int a, int b, int cap) {
    edge e1 = {a, b, cap, 0};
    edge e2 = {b, a, 0, 0};
    g[a].push_back((int)e.size());
    e.push_back(e1);
    g[b].push_back((int)e.size());
    e.push_back(e2);
}

bool bfs() {
    memset(d, -1, n * sizeof(d[0]));
    queue<int> q;
    q.push(s);
    d[s] = 0;
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (int id : g[v]) {
            int to = e[id].b;
            if (d[to] == -1 && e[id].flow < e[id].cap) {
                d[to] = d[v] + 1;
                q.push(to);
            }
        }
    }

    return d[t] != -1;
}

int dfs(int v, int flow) {
    if (!flow) return 0;
    if (v == t) return flow;

    for (; ptr[v] < g[v].size(); ++ptr[v]) {
        int id = g[v][ptr[v]], to = e[id].b;
        if (d[to] != d[v] + 1) continue;
        int pushed = dfs(to, min(flow, e[id].cap - e[id].flow));
        if (pushed) {
            e[id].flow += pushed;
            e[id ^ 1].flow -= pushed;
            return pushed;
        }
    }

    return 0;
}

int dinic() {
    int flow = 0;
    while (bfs()) {
        memset(ptr, 0, n * sizeof(ptr[0]));
        while (int pushed = dfs(s, INF)) flow += pushed;
    }

    return flow;
}

vector<int> find_path() {
    vector<int> path;
    int v = s;
    path.push_back(v);
    while (v != t) {
        for (int id : g[v]) {
            if (e[id].flow > 0) {
                e[id].flow--;
                v = e[id].b;
                path.push_back(v);
                break;
            }
        }
    }

    return path;
}

void solve() {
    int m;
    cin >> n >> m >> s >> t;
    --s, --t;

    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        add_edge(u - 1, v - 1, 1);
    }

    if (dinic() < 2) {
        cout << "NO" << endl;
        return;
    }

    cout << "YES" << endl;
    for (int i = 0; i < 2; i++) {
        for (int v : find_path()) {
            cout << v + 1 << " ";
        }
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

