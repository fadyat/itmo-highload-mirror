#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class segment_tree {
   private:
    int n;
    vector<vector<int>> tree;

    void build(const vector<int>& a, int v, int tl, int tr) {
        if (tl == tr) {
            tree[v] = {a[tl]};
        } else {
            int tm = (tl + tr) / 2;
            build(a, 2 * v, tl, tm);
            build(a, 2 * v + 1, tm + 1, tr);

            merge(tree[2 * v].begin(), tree[2 * v].end(),          
                  tree[2 * v + 1].begin(), tree[2 * v + 1].end(),  
                  back_inserter(tree[v]));
        }
    }

    int query(int v, int tl, int tr, int l, int r, int lval, int rval) {
        if (tl > r || tr < l) return 0;
        if (l <= tl && tr <= r) {
            return upper_bound(tree[v].begin(), tree[v].end(), rval) -
                   lower_bound(tree[v].begin(), tree[v].end(), lval);
        }

        int tm = (tl + tr) / 2;
        return query(2 * v, tl, tm, l, r, lval, rval) + 
               query(2 * v + 1, tm + 1, tr, l, r, lval, rval);
    }

   public:
    segment_tree(vector<int>& a) {
        n = a.size();
        tree.resize(4 * n);
        build(a, 1, 0, n - 1);
    }

    int query(int l, int r, int lval, int rval) { return query(1, 0, n - 1, l, r, lval, rval); }
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
        int x, y, k, l;
        cin >> x >> y >> k >> l;
        
        --x; --y;
        cout << tr.query(x, y, k, l) << endl;
    }
}

signed main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

