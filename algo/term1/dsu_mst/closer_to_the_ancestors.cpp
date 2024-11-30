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

    int get_size(int u) { return size[find(u)]; }
};

void solve() {
    int n;
    cin >> n;

    vector<int> p(n, -1);
    for (int i = 1; i < n; i++) {
        cin >> p[i];
    }

    vector<int> order(n - 1);
    for (int i = 0; i < n - 1; i++) {
        cin >> order[i];
    }

    DSU dsu(n);
    int max_size = 1;
    for (int i = 0; i < order.size(); i++) {
        dsu.unite(order[i], p[order[i]]);
        max_size = max(max_size, dsu.get_size(order[i]));
        cout << max_size << " ";
    }

    cout << endl;
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    int t;
    cin >> t;

    while (t--) {
        solve();
    }
}
