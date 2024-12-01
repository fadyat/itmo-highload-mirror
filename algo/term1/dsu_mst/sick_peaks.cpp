#include <iostream>
#include <vector>

using namespace std;

enum class QueryType { SICK, FIND };

struct Node {
    int idx;
    Node *parent;
    bool is_sick = false;
    int parent_before_sick = -1;

    Node(int idx, Node *parent) : idx(idx), parent(parent) {}

    void sick() {
        is_sick = true;
        parent_before_sick = parent == nullptr ? -1 : parent->idx;
        parent = nullptr;
    }
};

vector<Node *> nodes;

Node *find(int idx) {
    return nodes[idx]->parent == nullptr ? nodes[idx] : nodes[idx]->parent = find(nodes[idx]->parent->idx);
}

void unite(int idx, int pidx) {
    if (idx == 0) {
        nodes[idx]->is_sick = false;
        return;
    }

    Node *child = find(idx);
    Node *parent = find(pidx);

    if (child->parent == nullptr) {
        child->parent = parent;
        child->is_sick = false;
    }
}

Node *get_closest_sick(int idx) {
    if (nodes[idx]->is_sick) {
        return nodes[idx];
    }

    Node *parent = find(idx);
    return parent == nullptr ? nullptr : parent->is_sick ? parent : nullptr;
}

void solve() {
    int e, q;
    cin >> e >> q;

    nodes.resize(e);
    nodes[0] = new Node(0, nullptr);

    for (int i = 1; i < e; i++) {
        int p;
        cin >> p;

        nodes[i] = new Node(i, nodes[--p]);
    }

    vector<pair<QueryType, int>> queries;
    for (int i = 0; i < q; i++) {
        string type;
        int v;

        cin >> type >> v;
        v--;

        if (type == "-") {
            nodes[v]->sick();
            queries.push_back({QueryType::SICK, v});
        } else {
            queries.push_back({QueryType::FIND, v});
        }
    }

    vector<int> result;
    for (auto it = queries.rbegin(); it != queries.rend(); it++) {
        if (it->first == QueryType::SICK) {
            unite(it->second, nodes[it->second]->parent_before_sick);
        } else {
            Node *closest_sick = get_closest_sick(it->second);
            result.push_back(closest_sick == nullptr ? -1 : closest_sick->idx + 1);
        }
    }

    for (auto it = result.rbegin(); it != result.rend(); it++) {
        cout << *it << " ";
    }
    cout << endl;
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    int t;
    cin >> t;
    while (t--) {
        solve();
    }
}
