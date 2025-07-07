#include <iostream>

using namespace std;

typedef int value_t;

struct node {
    value_t value;
    node* left;
    node* right;
};

node* insert(node* root, value_t value) {
    if (root == nullptr) {
        root = new node();
        root->value = value;
        return root;
    }

    if (value < root->value) {
        root->left = insert(root->left, value);
    } else if (value > root->value) {
        root->right = insert(root->right, value);
    }

    return root;
}

value_t find_min_greater(node* root, value_t value) {
    value_t result = 0;
    while (root != nullptr) {
        if (root->value > value) {
            result = root->value;
            root = root->left;
        } else {
            root = root->right;
        }
    }

    return result;
}

void solve() {
    node* root = nullptr;
    string command;

    while (cin >> command) {
        if (command == "+") {
            value_t value;
            cin >> value;
            root = insert(root, value);
        } else if (command == ">") {
            value_t value;
            cin >> value;
            cout << find_min_greater(root, value) << endl;
        }
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

