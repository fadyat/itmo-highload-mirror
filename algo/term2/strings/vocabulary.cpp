#include <iostream>
#include <vector>

using namespace std;

constexpr int MAX_WORD_LENGTH = 30;
constexpr int ALPHABET_SIZE = 26;

struct vertex {
    bool is_leaf;
    int insert_idx;
    vertex* children[ALPHABET_SIZE];

    vertex() {
        is_leaf = false;
        insert_idx = -1;

        for (int i = 0; i < ALPHABET_SIZE; i++) {
            children[i] = nullptr;
        }
    }
};

class trie {
   private:
    vertex* root;

   public:
    trie() { root = new vertex(); }

    void insert(const string& s, int insert_idx) {
        vertex* cur = root;
        for (char c : s) {
            int l = c - 'a';
            if (cur->children[l] == nullptr) {
                cur->children[l] = new vertex();
            }

            cur = cur->children[l];
        }

        cur->insert_idx = insert_idx;
        cur->is_leaf = true;
    }

    vector<int> find(const string& s) {
        vertex* cur = root;
        vector<int> result;

        for (char c : s) {
            int l = c - 'a';
            if (cur->children[l] == nullptr) {
                return result;
            }

            cur = cur->children[l];
            if (cur->is_leaf) {
                result.push_back(cur->insert_idx);
            }
        }

        return result;
    }
};

void solve() {
    string t;
    cin >> t;
    int m;
    cin >> m;

    trie t_trie;
    for (int i = 0; i < m; i++) {
        string s;
        cin >> s;
        t_trie.insert(s, i);
    }

    vector<bool> found(m, false);
    for (int i = 0; i < t.size(); i++) {
        int right = min(i + MAX_WORD_LENGTH, static_cast<int>(t.size()));
        string substring = t.substr(i, right - i);
        for (int idx : t_trie.find(substring)) {
            found[idx] = true;
        }
    }

    for (int i = 0; i < m; i++) {
        cout << (found[i] ? "Yes" : "No") << endl;
    }
}

signed main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

