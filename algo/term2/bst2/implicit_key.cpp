#include <cstddef>
#include <iostream>

using namespace std;

struct node {
    int key, size;
    node *left, *right;

    node(int key) : key(key), size(1), left(nullptr), right(nullptr) {}
};

int get_size(node *v) { return v ? v->size : 0; }

void update_size(node *v) {
    if (v) {
        v->size = get_size(v->left) + get_size(v->right) + 1;
    }
};

node *insert(node *v, int idx, int key) {
    if (!v) {
        return new node(key);
    }

    int left_size = get_size(v->left);
    if (idx <= left_size) {
        v->left = insert(v->left, idx, key);
    } else {
        v->right = insert(v->right, idx - left_size - 1, key);
    }

    update_size(v);
    return v;
}

void print(node *v) {
    if (!v) {
        return;
    }

    print(v->left);
    cout << v->key << ' ';
    print(v->right);
}

void solve() {
    node *root = nullptr;

    int idx, key;
    while (cin >> idx >> key) {
        root = insert(root, idx, key);
    }

    print(root);
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

