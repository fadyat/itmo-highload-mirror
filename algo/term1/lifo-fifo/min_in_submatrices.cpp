#include <deque>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n, l;
    cin >> n >> l;

    vector<vector<int>> matrix(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> matrix[i][j];
        }
    }

    vector<vector<int>> row_min(n, vector<int>(n - l + 1));
    for (int i = 0; i < n; i++) {
        deque<int> window;
        for (int j = 0; j < n; j++) {
            while (!window.empty() && window.front() < j - l + 1) {
                window.pop_front();
            }
            while (!window.empty() && matrix[i][window.back()] > matrix[i][j]) {
                window.pop_back();
            }
            window.push_back(j);
            if (j >= l - 1) {
                row_min[i][j - l + 1] = matrix[i][window.front()];
            }
        }
    }

    vector<vector<int>> col_min(n - l + 1, vector<int>(n - l + 1));
    for (int j = 0; j < n - l + 1; j++) {
        deque<int> window;
        for (int i = 0; i < n; i++) {
            while (!window.empty() && window.front() < i - l + 1) {
                window.pop_front();
            }
            while (!window.empty() && row_min[window.back()][j] > row_min[i][j]) {
                window.pop_back();
            }
            window.push_back(i);
            if (i >= l - 1) {
                col_min[i - l + 1][j] = row_min[window.front()][j];
            }
        }
    }

    for (int i = 0; i < n - l + 1; i++) {
        for (int j = 0; j < n - l + 1; j++) {
            cout << col_min[i][j] << " ";
        }
        cout << "\n";
    }
}
