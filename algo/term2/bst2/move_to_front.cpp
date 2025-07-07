#include <iostream>

using namespace std;

struct node {
    node *left, *right;
    int x, y;
    int size;

    node(int x) : left(nullptr), right(nullptr), x(x), y(rand()), size(1) {}
};

class treap {
   private:
    int _size(node* v) { return v ? v->size : 0; }

    node* update(node* v) {
        if (!v) return nullptr;

        v->size = 1 + _size(v->left) + _size(v->right);
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

    void print(node* v) {
        if (!v) return;
        print(v->left);
        cout << v->x << " ";
        print(v->right);
    }

   public:
    void insert(int idx, int x) {
        auto [a, b] = split(root, idx);
        root = merge(merge(a, new node(x)), b);
    }

    void rotate_to_begining(int l, int r) {
        auto [a, b] = split(root, l);
        auto [c, d] = split(b, r - l + 1);

        root = merge(merge(c, a), d);
    }

    void print() { print(root); }
};

void solve() {
    treap tree;

    int n, m;
    cin >> n >> m;

    for (int i = 0; i < n; i++) {
        tree.insert(i, i + 1);
    }

    for (int i = 0; i < m; i++) {
        int l, r;
        cin >> l >> r;
        tree.rotate_to_begining(l - 1, r - 1);
    }

    tree.print();
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

