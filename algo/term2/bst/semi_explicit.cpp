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

value_t find_kth(node* root, size_t k) {
    if (root == nullptr) {
        return -1;
    }

    size_t left_size = root->left ? root->left->size : 0;
    if (k == left_size + 1) {
        return root->value;
    } else if (k <= left_size) {
        return find_kth(root->left, k);
    } else {
        return find_kth(root->right, k - left_size - 1);
    }
}

void solve() {
    node* root = nullptr;
    string command;

    while (cin >> command) {
        if (command == "+") {
            value_t value;
            cin >> value;
            root = insert(root, value);
        } else if (command == "?") {
            size_t k;
            cin >> k;
            cout << find_kth(root, k) << endl;
        }
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

