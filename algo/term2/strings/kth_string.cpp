#include <iostream>

using namespace std;

constexpr int ALPHABET_SIZE = 26;

struct vertex {
    int count = 0;
    bool is_leaf = false;
    vertex* children[ALPHABET_SIZE] = {nullptr};
};

class trie {
   private:
    vertex* root;

   public:
    trie() { root = new vertex(); }

    void insert(const string& s) {
        vertex* cur = root;
        for (char c : s) {
            int l = c - 'a';
            if (cur->children[l] == nullptr) {
                cur->children[l] = new vertex();
            }

            cur->count++;
            cur = cur->children[l];
        }

        if (!cur->is_leaf) {
            cur->count++;
            cur->is_leaf = true;
        } else {
            vertex* node = root;
            node->count--;
            for (char c : s) {
                node = node->children[c - 'a'];
                node->count--;
            }
        }
    }

    string kth(int k) {
        string result;
        vertex* cur = root;

        while (k > 0) {
            for (int i = 0; i < ALPHABET_SIZE; ++i) {
                vertex* child = cur->children[i];
                if (child) {
                    if (child->count >= k) {
                        cur = child;
                        result.push_back('a' + i);
                        if (cur->is_leaf) k--;
                        break;
                    } else {
                        k -= child->count;
                    }
                }
            }
        }

        return result;
    }
};

void solve() {
    int n;
    cin >> n;

    trie t_trie;
    for (int i = 0; i < n; i++) {
        string line;
        cin >> line;
        if (isdigit(line[0])) {
            int k = stoi(line);
            cout << t_trie.kth(k) << "\n";
        } else {
            t_trie.insert(line);
        }
    }
}

signed main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

