#include <iostream>
#include <vector>

using namespace std;

struct Edge {
    int u, v, w;

    bool operator<(const Edge &e) const { return w < e.w; }
};

class DSU {
  private:
    vector<int> parent, size;

  public:
    DSU(int n) : parent(n), size(n, 1) {
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }

    int find(int u) { return parent[u] == u ? u : parent[u] = find(parent[u]); }

    bool unite(int u, int v) {
        u = find(u);
        v = find(v);

        if (u == v) {
            return false;
        }

        if (size[u] < size[v]) {
            swap(u, v);
        }

        parent[v] = u;
        size[u] += size[v];
        return true;
    }
};

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    int v, e;
    cin >> v >> e;

    vector<Edge> edges(e);
    for (int i = 0; i < e; i++) {
        cin >> edges[i].u >> edges[i].v >> edges[i].w;
        --edges[i].u;
        --edges[i].v;
    }

    sort(edges.begin(), edges.end());

    // using Kruskal's algorithm with DSU to find the minimum spanning tree
    // DSU is implemented using tree representation with path compression
    DSU dsu(v);

    long long weight = 0;
    for (const auto &edge : edges) {
        if (dsu.unite(edge.u, edge.v)) {
            weight += edge.w;
        }
    }

    cout << weight << endl;
}
