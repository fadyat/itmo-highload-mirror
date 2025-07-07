#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class segment_tree {
   private:
    int n;
    vector<int> tree;

   public:
    segment_tree(int n) : n(n) { tree.resize(4 * n, 0); }

    void update(int node, int start, int end, int idx, int val) {
        if (start == end) {
            tree[node] += val;
        } else {
            int mid = (start + end) / 2;
            if (start <= idx && idx <= mid) {
                update(2 * node, start, mid, idx, val);
            } else {
                update(2 * node + 1, mid + 1, end, idx, val);
            }

            tree[node] = tree[2 * node] + tree[2 * node + 1];
        }
    }

    int query(int node, int start, int end, int l, int r) {
        if (r < start || end < l) {
            return 0;
        }

        if (l <= start && end <= r) {
            return tree[node];
        }

        int mid = (start + end) / 2;
        int left_query = query(2 * node, start, mid, l, r);
        int right_query = query(2 * node + 1, mid + 1, end, l, r);
        return left_query + right_query;
    }
};

vector<int> compress(vector<int>& ys) {
    vector<int> sorted_ys = ys;
    sort(sorted_ys.begin(), sorted_ys.end());
    sorted_ys.erase(unique(sorted_ys.begin(), sorted_ys.end()), sorted_ys.end());

    vector<int> compressed;
    for (int y : ys) {
        compressed.push_back(lower_bound(sorted_ys.begin(), sorted_ys.end(), y) - sorted_ys.begin() + 1);
    }

    return compressed;
}

void solve() {
    int n;
    while (cin >> n) {
        vector<pair<int, int>> stars(n);
        vector<int> ys(n);

        for (int i = 0; i < n; i++) {
            cin >> stars[i].first >> stars[i].second;
            ys[i] = stars[i].second;
        }

        vector<int> compressed_ys = compress(ys);
        for (int i = 0; i < n; i++) {
            stars[i].second = compressed_ys[i];
        }

        sort(stars.begin(), stars.end());
        segment_tree t(n);
        vector<int> levels(n);
        
        for (auto [x, y] : stars) {
            int level = t.query(1, 1, n, 1, y);
            levels[level]++;
            t.update(1, 1, n, y, 1);
        }

        for (int i = 0; i < n; i++) {
            cout << levels[i] << endl;
        }
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

