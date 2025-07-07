#include <cstddef>
#include <iostream>

using namespace std;

typedef int value_t;

struct node {
    value_t value;
    node* left;
    node* right;
    size_t size;
};

node* insert(node* root, value_t value) {
    if (root == nullptr) {
        return new node{value, nullptr, nullptr, 1};
    }

    if (value < root->value) {
        root->left = insert(root->left, value);
    } else if (value > root->value) {
        root->right = insert(root->right, value);
    }

    root->size = 1 + (root->left ? root->left->size : 0) + (root->right ? root->right->size : 0);
    return root;
}

void debug(node* root, size_t depth = 0) {
    if (root == nullptr) {
        return;
    }

    debug(root->left, depth + 1);
    cout << string(2 * depth, ' ') << "x=" << root->value << " size=" << root->size << endl;
    debug(root->right, depth + 1);
}

void solve() {
    node* root = nullptr;
    bool need_separator = false;

    value_t value;
    while (cin >> value) {
        root = insert(root, value);
        if (need_separator) {
            cout << '-' << endl;
        }

        need_separator = true;
        debug(root);
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

