#include <iostream>
#include <vector>

using namespace std;

class segment_tree {
   private:
    int n;
    vector<int> tree;

    void build(const vector<int>& a, int v, int tl, int tr) {
        if (tl == tr) {
            tree[v] = a[tl];
        } else {
            int tm = (tl + tr) / 2;
            build(a, 2 * v, tl, tm);
            build(a, 2 * v + 1, tm + 1, tr);
            tree[v] = max(tree[2 * v], tree[2 * v + 1]);
        }
    }

    void update(int pos, int val, int v, int tl, int tr) {
        if (tl == tr) {
            tree[v] = val;
        } else {
            int tm = (tl + tr) / 2;
            if (pos <= tm) {
                update(pos, val, 2 * v, tl, tm);
            } else {
                update(pos, val, 2 * v + 1, tm + 1, tr);
            }

            tree[v] = max(tree[2 * v], tree[2 * v + 1]);
        }
    }

    int query(int i, int x, int v, int tl, int tr) {
        if (tr < i || tree[v] < x) return -1;
        if (tl == tr) return tl;
        int tm = (tl + tr) / 2;
        int res = query(i, x, 2 * v, tl, tm);
        if (res == -1) res = query(i, x, 2 * v + 1, tm + 1, tr);
        return res;
    }

   public:
    segment_tree(vector<int>& a) {
        n = a.size();
        tree.assign(4 * n, 0);
        build(a, 1, 0, n - 1);
    }

    void update(int i, int x) { update(i, x, 1, 0, n - 1); }
    int query(int i, int x) { return query(i, x, 1, 0, n - 1); }
};

void solve() {
    int n, m;
    cin >> n >> m;

    vector<int> a(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }

    segment_tree tr(a);
    while (m--) {
        int t, i, x;
        cin >> t >> i >> x;
        i--;

        if (t == 1) {
            int res = tr.query(i, x);
            cout << (res == -1 ? -1 : res + 1) << endl;
        } else {
            tr.update(i, x);
        }
    }
}

signed main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

