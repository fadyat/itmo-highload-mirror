#include <iostream>
#include <vector>

using namespace std;

const double EPS = 1E-9;

double determinant(int n, vector<vector<double>> &a) {
    double det = 1;
    for (int i = 0; i < n; ++i) {
        int k = i;
        for (int j = i + 1; j < n; ++j) {
            if (abs(a[j][i]) > abs(a[k][i])) {
                k = j;
            }
        }

        if (abs(a[k][i]) < EPS) {
            det = 0;
            break;
        }

        if (i != k) {
            swap(a[i], a[k]);
            det = -det;
        }

        det *= a[i][i];
        for (int j = i + 1; j < n; ++j) {
            a[i][j] /= a[i][i];
        }

        for (int j = 0; j < n; ++j) {
            if (j != i && abs(a[j][i]) > EPS) {
                for (int k = i + 1; k < n; ++k) {
                    a[j][k] -= a[i][k] * a[j][i];
                }
            }
        }
    }

    return det;
}

void solve() {
    int n;
    while (cin >> n) {
        vector<vector<double>> a(n, vector<double>(n));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cin >> a[i][j];
            }
        }

        cout << determinant(n, a) << endl;
    }
}

int main() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);

    solve();
}

