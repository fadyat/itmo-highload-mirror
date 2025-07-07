#include <cstring>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

struct aho_corasick {
    struct node {
        unordered_map<char, int> next;
        int suffix_link = -1;
        unordered_set<int> out;
    };

    vector<node> trie;

    aho_corasick() { trie.emplace_back(); }

    void add_word(const string& word, int idx) {
        int state = 0;
        for (char c : word) {
            if (!trie[state].next.count(c)) {
                trie[state].next[c] = trie.size();
                trie.emplace_back();
            }

            state = trie[state].next[c];
        }

        trie[state].out.insert(idx);
    }

    void build() {
        queue<int> q;
        for (const auto& [c, next_state] : trie[0].next) {
            trie[next_state].suffix_link = 0;
            q.push(next_state);
        }

        while (!q.empty()) {
            int state = q.front();
            q.pop();

            for (const auto& [c, next_state] : trie[state].next) {
                q.push(next_state);

                int suffix_link = trie[state].suffix_link;
                while (suffix_link != -1 && !trie[suffix_link].next.count(c)) {
                    suffix_link = trie[suffix_link].suffix_link;
                }

                trie[next_state].suffix_link = (suffix_link == -1) ? 0 : trie[suffix_link].next[c];

                for (int out_idx : trie[trie[next_state].suffix_link].out) {
                    trie[next_state].out.insert(out_idx);
                }
            }
        }
    }

    bool search(const string& text) {
        int state = 0;

        for (char c : text) {
            while (state != -1 && !trie[state].next.count(c)) {
                state = trie[state].suffix_link;
            }

            state = (state == -1) ? 0 : trie[state].next[c];
            if (!trie[state].out.empty()) {
                return true;
            }
        }

        return false;
    }
};

void solve() {
    int n;
    cin >> n;
    cin.ignore();

    vector<string> T(n);
    for (int i = 0; i < n; i++) {
        getline(cin, T[i]);
    }

    aho_corasick ac;
    for (int i = 0; i < n; i++) {
        ac.add_word(T[i], i);
    }

    ac.build();

    string line;
    while (getline(cin, line)) {
        if (ac.search(line)) {
            cout << line << endl;
        }
    }
}

signed main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

