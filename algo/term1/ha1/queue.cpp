#include <iostream>
#include <queue>

using namespace std;

struct human {
    int time;
    int productType;
};

int main() {
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);

    int n;
    cin >> n;

    queue<human> q;
    vector<int> productCount(10001, 0);
    vector<int> humanWaitTime;

    auto processQueue = [&](int currentTime) {
        while (!q.empty() && productCount[q.front().productType] > 0) {
            human h = q.front();
            q.pop();
            humanWaitTime.push_back(max(0, currentTime - h.time));
            productCount[h.productType]--;
        }
    };

    while (n--) {
        int eventType, timeInterval, productType;
        cin >> eventType >> timeInterval >> productType;

        if (eventType == 1) {
            productCount[productType]++;
        } else {
            q.push({timeInterval, productType});
        }

        processQueue(timeInterval);
    }

    humanWaitTime.resize(humanWaitTime.size() + q.size(), -1);
    for (int time : humanWaitTime) {
        cout << time << " ";
    }
}
