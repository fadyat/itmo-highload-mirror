#include <iostream>
#include <vector>

using namespace std;

void actual_main() {
    int n;
    cin >> n;

    vector<vector<int>> d(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> d[i][j];
        }
    }

    for (int k = 0; k < n; k++) {
        for (int u = 0; u < n; u++) {
            for (int v = 0; v < n; v++) {
                d[u][v] = min(d[u][v], d[u][k] + d[k][v]);
            }
        }
    }

    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            cout << d[u][v] << " ";
        }
        cout << endl;
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    actual_main();
}

