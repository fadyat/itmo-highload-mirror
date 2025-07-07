#include <iostream>

using namespace std;

struct node {
    node *left, *right;
    int x, y;
    int size, min;

    node(int x) : left(nullptr), right(nullptr), x(x), y(rand()), size(1), min(x) {}
};

class treap {
   private:
    int _size(node* v) { return v ? v->size : 0; }
    int _min(node* v) { return v ? v->min : INT_MAX; }

    node* update(node* v) {
        if (!v) return nullptr;

        v->size = 1 + _size(v->left) + _size(v->right);
        v->min = min(v->x, min(_min(v->left), _min(v->right)));
        return v;
    }

   private:
    node* root = nullptr;

    pair<node*, node*> split(node* v, int idx) {
        if (!v) return {nullptr, nullptr};

        if (idx <= _size(v->left)) {
            auto [a, b] = split(v->left, idx);
            v->left = b;
            return {a, update(v)};
        } else {
            auto [a, b] = split(v->right, idx - _size(v->left) - 1);
            v->right = a;
            return {update(v), b};
        }
    }

    node* merge(node* a, node* b) {
        if (!a) return b;
        if (!b) return a;

        if (a->y > b->y) {
            a->right = merge(a->right, b);
            return update(a);
        } else {
            b->left = merge(a, b->left);
            return update(b);
        }
    }

   public:
    void insert(int idx, int x) {
        auto [a, b] = split(root, idx);
        root = merge(merge(a, new node(x)), b);
    }

    int range_min(int l, int r) {
        auto [a, b] = split(root, l);
        auto [c, d] = split(b, r - l + 1);
        int res = _min(c);
        root = merge(merge(a, c), d);
        return res;
    }
};

void solve() {
    treap tree;

    int n;
    cin >> n;

    while (n--) {
        char c;
        cin >> c;

        if (c == '+') {
            int i, x;
            cin >> i >> x;
            tree.insert(i, x);
        } else {
            int i, j;
            cin >> i >> j;
            cout << tree.range_min(i - 1, j - 1) << endl;
        }
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

