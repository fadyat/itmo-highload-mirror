#include <cstring>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

using namespace std;

const int MAXN = 105;
const int INF = 1e9;

struct edge {
    int a, b, cap, flow;
};

vector<edge> e;
vector<int> g[MAXN];
int n, s, t, d[MAXN], ptr[MAXN];

void add_edge(int a, int b, int cap, int idx) {
    edge e1 = {a, b, cap, 0};
    edge e2 = {b, a, cap, 0};
    g[a].push_back(e.size());
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

void find_min_cut(set<int>& cut_indexes, int& cut_capacity) {
    vector<bool> visited(n, false);
    queue<int> q;
    q.push(s);
    visited[s] = true;

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (int id : g[v]) {
            int to = e[id].b;
            if (!visited[to] && e[id].flow < e[id].cap) {
                visited[to] = true;
                q.push(to);
            }
        }
    }

    for (int i = 0; i < e.size(); i++) {
        int u = e[i].a, v = e[i].b;
        if (visited[u] && !visited[v]) {
            cut_indexes.insert((i | 1) / 2);
            cut_capacity += e[i].cap;
        }
    }
}

void solve() {
    int m;
    cin >> n >> m;
    s = 0, t = n - 1;

    for (int i = 0; i < m; ++i) {
        int u, v, cap;
        cin >> u >> v >> cap;
        add_edge(u - 1, v - 1, cap, i);
    }

    dinic();

    set<int> cut_indexes;
    int cut_capacity = 0;
    find_min_cut(cut_indexes, cut_capacity);

    cout << cut_indexes.size() << " " << cut_capacity << endl;
    for (int idx : cut_indexes) {
        cout << idx + 1 << " ";
    }
    cout << endl;
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

