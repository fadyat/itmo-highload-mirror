#include <iostream>
#include <memory>
#include <queue>
#include <unordered_map>

using namespace std;

struct Node {
    char c;
    int freq;
    shared_ptr<Node> left;
    shared_ptr<Node> right;
};

struct Compare {
    bool operator()(const shared_ptr<Node> &a, const shared_ptr<Node> &b) const { return a->freq > b->freq; }
};

unordered_map<char, int> calculate_freq(string s) {
    unordered_map<char, int> freq;
    for (char c : s) {
        freq[c]++;
    }

    return freq;
}

shared_ptr<Node> build_tree(unordered_map<char, int> freq) {
    priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, Compare> q;
    for (auto [c, f] : freq) {
        q.push(make_shared<Node>(Node{c, f}));
    }

    while (q.size() > 1) {
        auto left = q.top();
        q.pop();
        auto right = q.top();
        q.pop();
        q.push(make_shared<Node>(Node{'\0', left->freq + right->freq, left, right}));
    }

    return q.top();
}

void _generate_codes(shared_ptr<Node> node, const string &prefix, unordered_map<char, string> &codes) {
    if (node == nullptr) {
        return;
    }

    if (node->left == nullptr && node->right == nullptr) {
        codes[node->c] = prefix;
        return;
    }

    _generate_codes(node->left, prefix + "0", codes);
    _generate_codes(node->right, prefix + "1", codes);
}

unordered_map<char, string> generate_codes(shared_ptr<Node> root) {
    if (root->left == nullptr && root->right == nullptr) {
        return {{root->c, "0"}};
    }

    unordered_map<char, string> codes;
    _generate_codes(root, "", codes);
    return codes;
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    string s;
    cin >> s;

    auto freq = calculate_freq(s);
    auto root = build_tree(freq);
    auto codes = generate_codes(root);

    string encoded;
    for (char c : s) {
        encoded += codes[c];
    }

    cout << codes.size() << " " << encoded.size() << endl;
    for (auto [c, code] : codes) {
        cout << c << ": " << code << endl;
    }

    cout << encoded << endl;
}
