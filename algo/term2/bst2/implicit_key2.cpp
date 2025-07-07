#include <iostream>

using namespace std;

struct node {
    int value, size, height;
    node *left, *right;

    node(int value) : value(value), size(1), height(1), left(nullptr), right(nullptr) {}
};

class avl {
   private:
    node* root = nullptr;

    int size(node* v) { return v ? v->size : 0; }
    int height(node* v) { return v ? v->height : 0; }

    void update(node* v) {
        v->size = size(v->left) + size(v->right) + 1;
        v->height = max(height(v->left), height(v->right)) + 1;
    }

    node* rright(node* v) {
        node* u = v->left;
        v->left = u->right;
        u->right = v;
        update(v);
        update(u);
        return u;
    }

    node* rleft(node* v) {
        node* u = v->right;
        v->right = u->left;
        u->left = v;
        update(v);
        update(u);
        return u;
    }

    node* balance(node* v) {
        update(v);

        if (height(v->left) - height(v->right) == 2) {
            if (height(v->left->right) > height(v->left->left)) {
                v->left = rleft(v->left);
            }
            return rright(v);
        }

        if (height(v->right) - height(v->left) == 2) {
            if (height(v->right->left) > height(v->right->right)) {
                v->right = rright(v->right);
            }
            return rleft(v);
        }

        return v;
    }

    node* insert(node* v, int idx, int x) {
        if (!v) {
            return new node(x);
        }

        int left_size = size(v->left);
        if (idx <= left_size) {
            v->left = insert(v->left, idx, x);
        } else {
            v->right = insert(v->right, idx - left_size - 1, x);
        }

        update(v);
        return balance(v);
    }

    node* find_min(node* v) { return v->left ? find_min(v->left) : v; }

    node* erase_min(node* v) {
        if (!v->left) return v->right;
        v->left = erase_min(v->left);
        update(v);
        return balance(v);
    }

    node* erase(node* v, int idx) {
        if (!v) return nullptr;

        int left_size = size(v->left);
        if (idx < left_size) {
            v->left = erase(v->left, idx);
        } else if (idx > left_size) {
            v->right = erase(v->right, idx - left_size - 1);
        } else {
            if (!v->left) return v->right;
            if (!v->right) return v->left;

            node* min_node = find_min(v->right);
            v->value = min_node->value;
            v->right = erase_min(v->right);
        }

        update(v);
        return balance(v);
    }

    void print(node* v) {
        if (!v) {
            return;
        }

        print(v->left);
        cout << v->value << " ";
        print(v->right);
    }

   public:
    void insert(int idx, int x) { root = insert(root, idx, x); }
    void erase(int x) { root = erase(root, x); }
    int size() { return size(root); }
    void print() { print(root); }
};

void solve() {
    int n, m;
    cin >> n >> m;

    avl root;
    for (int i = 0; i < n; i++) {
        int x;
        cin >> x;
        root.insert(i, x);
    }

    for (int i = 0; i < m; i++) {
        string cmd;
        cin >> cmd;
        if (cmd == "add") {
            int idx, key;
            cin >> idx >> key;
            root.insert(idx, key);
        } else if (cmd == "del") {
            int idx;
            cin >> idx;
            root.erase(idx - 1);
        }
    }

    cout << root.size() << endl;
    root.print();
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

