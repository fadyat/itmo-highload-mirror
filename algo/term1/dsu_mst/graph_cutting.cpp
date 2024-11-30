#include <iostream>
#include <vector>

using namespace std;

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

enum class QueryType { CUT, ASK };

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    int v, e, k;
    cin >> v >> e >> k;

    vector<pair<int, int>> edges(e);
    for (int i = 0; i < e; i++) {
        cin >> edges[i].first >> edges[i].second;
    }

    vector<pair<QueryType, pair<int, int>>> queries;
    for (int i = 0; i < k; i++) {
        string type;
        int u, v;

        cin >> type >> u >> v;
        if (type == "cut") {
            queries.push_back({QueryType::CUT, {u, v}});
        } else {
            queries.push_back({QueryType::ASK, {u, v}});
        }
    }

    DSU dsu(v + 1);
    vector<bool> result;

    for (auto it = queries.rbegin(); it != queries.rend(); it++) {
        auto [type, q] = *it;
        if (type == QueryType::CUT) {
            dsu.unite(q.first, q.second);
        } else {
            result.push_back(dsu.find(q.first) == dsu.find(q.second));
        }
    }

    for (auto it = result.rbegin(); it != result.rend(); it++) {
        cout << (*it ? "YES" : "NO") << endl;
    }
}
