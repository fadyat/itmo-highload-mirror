#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

struct node {
    int min_val;
    int max_val;
};

class segment_tree {
   private:
    vector<node> tree;
    int n;

   public:
    segment_tree(const vector<int>& a) {
        n = a.size();
        tree.resize(4 * n);
        build(a, 1, 0, n - 1);
    }

    node combine(const node& a, const node& b) {
        node res;
        res.min_val = min(a.min_val, b.min_val);
        res.max_val = max(a.max_val, b.max_val);
        return res;
    }

    void build(const vector<int>& a, int v, int tl, int tr) {
        if (tl == tr) {
            tree[v].min_val = a[tl];
            tree[v].max_val = a[tl];
        } else {
            int tm = (tl + tr) / 2;
            build(a, 2 * v, tl, tm);
            build(a, 2 * v + 1, tm + 1, tr);

            tree[v] = combine(tree[2 * v], tree[2 * v + 1]);
        }
    }

    void update(int v, int tl, int tr, int pos, int new_val) {
        if (tl == tr) {
            tree[v].min_val = new_val;
            tree[v].max_val = new_val;
        } else {
            int tm = (tl + tr) / 2;
            if (pos <= tm) {
                update(2 * v, tl, tm, pos, new_val);
            } else {
                update(2 * v + 1, tm + 1, tr, pos, new_val);
            }

            tree[v] = combine(tree[2 * v], tree[2 * v + 1]);
        }
    }

    node query(int v, int tl, int tr, int l, int r) {
        if (l > r) {
            return {INT_MAX, INT_MIN};
        }

        if (l == tl && r == tr) {
            return tree[v];
        }

        int tm = (tl + tr) / 2;
        return combine(query(2 * v, tl, tm, l, min(r, tm)), query(2 * v + 1, tm + 1, tr, max(l, tm + 1), r));
    }
};

void solve() {
    long long n = 100000;
    vector<int> a(n);
    for (long long i = 1; i <= n; ++i) {
        a[i - 1] = (i * i % 12345) + (i * i * i % 23456);
    }

    int k;
    cin >> k;

    segment_tree st(a);

    for (int i = 0; i < k; ++i) {
        int x, y;
        cin >> x >> y;
        if (x > 0) {
            auto res = st.query(1, 0, n - 1, x - 1, y - 1);
            cout << res.max_val - res.min_val << '\n';
        } else {
            st.update(1, 0, n - 1, abs(x) - 1, y);
        }
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

