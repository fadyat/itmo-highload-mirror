#include <iostream>
#include <vector>

using namespace std;

enum class QueryType { SICK, FIND };

class DSU {
  private:
    vector<int> parent, rank, nearest_sick;

  public:
    DSU(int n) : parent(n), rank(n, 0), nearest_sick(n) {
        for (int i = 0; i < n; i++) {
            parent[i] = i;
            nearest_sick[i] = -1;
        }
    }

    int find(int u) { return parent[u] == u ? u : parent[u] = find(parent[u]); }

    void unite(int u, int v) {
        u = find(u);
        v = find(v);

        if (u == v) {
            return;
        }

        if (rank[u] < rank[v]) {
            swap(u, v);
        }

        parent[v] = u;
        if (rank[u] == rank[v]) {
            rank[u]++;
        }

        // if (nearest_sick[v] != -1 && (nearest_sick[u] == -1 || nearest_sick[u] > nearest_sick[v])) {
        // nearest_sick[u] = nearest_sick[v];
        // }
        //

        // show();
    }

    int get_nearest_sick(int u) {
        while (u != -1) {
            if (nearest_sick[u] != -1) {
                return nearest_sick[u];
            }

            u = parent[u];
        }

        return -1;
    }

    void set_sick(int u) { nearest_sick[find(u)] = u; }

    void show() {
        for (int i = 0; i < parent.size(); i++) {
            cout << parent[i] << " ";
        }
        cout << "-----" << endl;
    }
};

void solve() {
    int e, q;
    cin >> e >> q;

    vector<int> p(e, -1);
    for (int i = 1; i < e; i++) {
        cin >> p[i];
        --p[i];
    }

    vector<pair<QueryType, int>> queries;
    for (int i = 0; i < q; i++) {
        string type;
        int v;

        cin >> type >> v;
        v--;

        if (type == "-") {
            queries.push_back({QueryType::SICK, v});
        } else {
            queries.push_back({QueryType::FIND, v});
        }
    }

    // dsu должно состоять из множеств живых вершин, потом мы инвертируем заболевание за счет
    // обратного порядка запросов и объединения множеств
    DSU dsu(e);
    vector<int> result;

    // building dsu
    for (int i = 0; i < e; i++) {
        dsu.unite(i, p[i]);
    }

    dsu.show();
    for (auto it = queries.rbegin(); it != queries.rend(); it++) {
        if (it->first == QueryType::SICK) {
            dsu.set_sick(it->second);
        } else {
            result.push_back(dsu.get_nearest_sick(it->second));
        }
    }

    for (auto it = result.rbegin(); it != result.rend(); it++) {
        cout << *it << " ";
    }
    cout << endl;
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    // Случилось страшное, в древнем великом дереве вершины начали заболевать.
    // Вы пока не понимаете причину болезни, пытаетесь разобраться,
    // для этого нужно уметь быстро узнавать ближайшую к 𝑖 в направлении корня больную вершину.

    int t;
    cin >> t;
    while (t--) {
        solve();
    }
}
