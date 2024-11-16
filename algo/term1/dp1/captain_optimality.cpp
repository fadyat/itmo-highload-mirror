#include <iostream>
#include <map>
#include <queue>

#define ull unsigned long long

using namespace std;

void solve(ull n, ull a, ull b, ull c) {
    map<ull, ull> dp;
    dp[n] = 0;

    queue<ull> q;
    q.push(n);

    while (!q.empty()) {
        ull cur = q.front();
        q.pop();
        
        if (cur == 1) break;
        
        vector<ull> nexts;
        if (cur % a == 0) nexts.push_back(cur / a);
        if (cur % b == 0) nexts.push_back(cur / b);
        if (cur > c) nexts.push_back(cur - c);
        if (cur > 1) nexts.push_back(cur - 1);
        
        for (auto next : nexts) {
            if (dp.find(next) == dp.end()) {
                dp[next] = dp[cur] + 1;
                q.push(next);
            }
        }
    }

    vector<ull> path;
    ull cur = 1;
    while (cur != n) {
        path.push_back(cur);

        vector<ull> nexts;
        if (cur <= n / a) nexts.push_back(cur * a);
        if (cur <= n / b) nexts.push_back(cur * b);
        if (cur <= n - c) nexts.push_back(cur + c);
        if (cur <= n - 1) nexts.push_back(cur + 1);
        
        for (auto next : nexts) {
            if (dp.find(next) != dp.end() && dp[next] == dp[cur] - 1) {
                cur = next;
                break;
            }
        }
    }
    path.push_back(n);
    
    cout << path.size() << " ";
    for (int i = 0; i < path.size(); i++) {
        cout << path[i] << " ";
    }
    cout << endl;
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    ull n, a, b, c;
    while (cin >> n >> a >> b >> c) {
        solve(n, a, b, c);
    }
}
