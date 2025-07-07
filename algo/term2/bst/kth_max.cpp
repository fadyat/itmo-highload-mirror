#include <iostream>

using namespace std;

struct node {
    int value, size, height;
    node *left, *right;
    bool is_deleted = false;

    node(int value) : value(value), size(1), height(1), left(nullptr), right(nullptr) {}
};

class avl {
   private:
    node* root = nullptr;

    int size(node* v) { return v ? v->size : 0; }
    int height(node* v) { return v ? v->height : 0; }

    void update(node* v) {
        v->size = size(v->left) + size(v->right) + (v->is_deleted ? 0 : 1);
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

    node* insert(node* v, int x) {
        if (!v) {
            return new node(x);
        }

        if (x < v->value) {
            v->left = insert(v->left, x);
        } else {
            v->right = insert(v->right, x);
        }

        return balance(v);
    }

    node* erase(node* v, int x) {
        if (!v) {
            return nullptr;
        }

        if (x < v->value) {
            v->left = erase(v->left, x);
        } else if (x > v->value) {
            v->right = erase(v->right, x);
        } else {
            v->is_deleted = true;
        }

        update(v);
        return balance(v);
    }

    node* find_kth_max(node* v, int k) {
        if (!v) {
            return nullptr;
        }

        if (size(v->right) + 1 == k && !v->is_deleted) {
            return v;
        }

        if (size(v->right) >= k) {
            return find_kth_max(v->right, k);
        }

        return find_kth_max(v->left, k - size(v->right) - (v->is_deleted ? 0 : 1));
    }

   public:
    void insert(int x) { root = insert(root, x); }
    void erase(int x) { root = erase(root, x); }
    int find_kth_max(int k) { return find_kth_max(root, k)->value; }
};

void solve() {
    int n;
    cin >> n;
    avl tree;

    while (n--) {
        int cmd, k;
        cin >> cmd >> k;
        if (cmd == 1) {
            tree.insert(k);
        } else if (cmd == -1) {
            tree.erase(k);
        } else if (cmd == 0) {
            cout << tree.find_kth_max(k) << endl;
        }
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

