#include <climits>
#include <iostream>
#include <vector>

using namespace std;

// Stores which lids will be available if we buy this set at the specified price
struct lids_set {
    int price;
    int masks;

    lids_set(int price, int masks) : price(price), masks(masks) {}
};

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n;
    cin >> n;

    vector<lids_set> lids_sets;
    for (int i = 0; i < n; i++) {
        int price;
        cin >> price;
        lids_sets.push_back(lids_set(price, 1 << i));
    }

    int m;
    cin >> m;

    for (int i = 0; i < m; i++) {
        int price, quantity;
        cin >> price >> quantity;

        int masks = 0;
        for (int j = 0; j < quantity; j++) {
            int lid;
            cin >> lid;
            masks |= 1 << (lid - 1);
        }

        lids_sets.push_back(lids_set(price, masks));
    }

    int quantity;
    int required_lids = 0;
    cin >> quantity;

    for (int i = 0; i < quantity; i++) {
        int lid;
        cin >> lid;
        required_lids |= 1 << (lid - 1);
    }

    vector<int> dp(1 << n, INT_MAX);
    dp[0] = 0;
    for (int i = 0; i < lids_sets.size(); i++) {
        dp[lids_sets[i].masks] = lids_sets[i].price;
    }

    for (int mask = 0; mask < (1 << n); mask++) {
        for (int i = 0; i < lids_sets.size(); i++) {
            int new_mask = mask | lids_sets[i].masks;
            dp[new_mask] = min(dp[new_mask], dp[mask] + lids_sets[i].price);
        }
    }

    // also we can buy more lids than required
    int answer = dp[required_lids];
    for (int mask = required_lids; mask < (1 << n); mask++) {
        if ((mask | required_lids) == mask) {
            answer = min(answer, dp[mask]);
        }
    }

    std::cout << (answer == INT_MAX ? -1 : answer) << std::endl;
}
