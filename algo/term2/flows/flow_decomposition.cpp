#include <cstring>
#include <functional>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

#define int long long

const int MAXN = 505;
const long long INF = 1e15;

struct edge {
    int a, b, cap, flow;
};

vector<edge> e;
vector<int> g[MAXN];
int n, s, t, d[MAXN], ptr[MAXN];

void add_edge(int a, int b, int cap, int idx) {
    edge e1 = {a, b, cap, 0};
    edge e2 = {b, a, 0, 0};
    g[a].push_back(e.size());
    e.push_back(e1);
    g[b].push_back(e.size());
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
        long long pushed = dfs(to, min(flow, e[id].cap - e[id].flow));
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

int find_path(int v, vector<int>& path_edges) {
    if (v == t) {
        int path_flow_value = INF;
        for (int i = 0; i < path_edges.size(); ++i) {
            int id = path_edges[i];
            path_flow_value = min(path_flow_value, e[id].flow);
        }

        return path_flow_value;
    }

    for (int id : g[v]) {
        int to = e[id].b;
        if (e[id].flow > 0) {
            path_edges.push_back(id);
            int path_flow_value = find_path(to, path_edges);
            if (path_flow_value > 0) {
                return path_flow_value;
            }

            path_edges.pop_back();
        }
    }

    return -1;
}

void print_paths() {
    vector<vector<int>> paths;
    vector<int> paths_flow;

    vector<int> path_edges;
    int path_flow = find_path(s, path_edges);

    while (path_flow > 0) {
        paths_flow.push_back(path_flow);
        paths.push_back(path_edges);

        for (int id : path_edges) {
            e[id].flow -= path_flow;
            e[id ^ 1].flow += path_flow;
        }

        path_edges.clear();
        path_flow = find_path(s, path_edges);
    }

    cout << paths.size() << endl;
    for (int i = 0; i < paths.size(); ++i) {
        cout << paths_flow[i] << " " << paths[i].size() << " ";
        for (int j = 0; j < paths[i].size(); ++j) {
            cout << ((paths[i][j] | 1) / 2) + 1 << " ";
        }
        cout << endl;
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
    print_paths();
}

signed main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

