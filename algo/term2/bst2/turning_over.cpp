#include <cstdlib>
#include <ctime>
#include <iostream>
#include <utility>

using namespace std;

struct node {
    node *left, *right;
    int x, y;
    int size;
    long long sum;
    bool reverse;

    node(int x) : left(nullptr), right(nullptr), x(x), y(rand()), size(1), sum(x), reverse(false) {}
};

class treap {
   private:
    int _size(node* v) { return v ? v->size : 0; }

    long long _sum(node* v) { return v ? v->sum : 0; }

    void push(node* v) {
        if (v && v->reverse) {
            v->reverse = false;
            swap(v->left, v->right);
            if (v->left) v->left->reverse ^= true;
            if (v->right) v->right->reverse ^= true;
        }
    }

    node* update(node* v) {
        if (!v) return nullptr;
        push(v);
        v->size = 1 + _size(v->left) + _size(v->right);
        v->sum = v->x + _sum(v->left) + _sum(v->right);
        return v;
    }

   private:
    node* root = nullptr;

    pair<node*, node*> split(node* v, int idx) {
        if (!v) return {nullptr, nullptr};
        push(v);

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
        push(a);
        push(b);

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

    long long query_sum(int l, int r) {
        auto [a, b] = split(root, l - 1);
        auto [c, d] = split(b, r - l + 1);
        long long sum = _sum(c);
        root = merge(a, merge(c, d));
        return sum;
    }

    void reverse_range(int l, int r) {
        auto [a, b] = split(root, l - 1);
        auto [c, d] = split(b, r - l + 1);
        if (c) c->reverse ^= true;
        root = merge(a, merge(c, d));
    }

    void print(node* v) {
        if (!v) return;
        push(v);
        print(v->left);
        cout << v->x << " ";
        print(v->right);
    }

    void print() {
        print(root);
        cout << endl;
    }
};

void solve() {
    int n, m;
    cin >> n >> m;

    treap t;
    for (int i = 0; i < n; ++i) {
        int x;
        cin >> x;
        t.insert(i, x);
    }

    for (int i = 0; i < m; ++i) {
        int q, l, r;
        cin >> q >> l >> r;
        if (q == 0) {
            cout << t.query_sum(l, r) << endl;
        } else {
            t.reverse_range(l, r);
        }
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

