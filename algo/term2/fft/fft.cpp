#include <algorithm>
#include <complex>
#include <iostream>
#include <vector>

using namespace std;

const double PI = acos(-1);

vector<complex<double>> fft(const vector<complex<double>> &a, int n) {
    if (n == 1) {
        return a;
    }

    vector<complex<double>> a0(n / 2), a1(n / 2);
    for (int i = 0; i < n / 2; i++) {
        a0[i] = a[i * 2];
        a1[i] = a[i * 2 + 1];
    }

    auto f0 = fft(a0, n / 2);
    auto f1 = fft(a1, n / 2);

    vector<complex<double>> fa(n);
    vector<complex<double>> w(n);
    for (int i = 0; i < n; i++) {
        w[i] = complex<double>(cos(2 * PI / n * i), sin(2 * PI / n * i));
    }

    for (int i = 0; i < n / 2; i++) {
        fa[i] = f0[i] + w[i] * f1[i];
        fa[i + n / 2] = f0[i] - w[i] * f1[i];
    }

    return fa;
}

vector<complex<double>> rfft(const vector<complex<double>> &f, int n) {
    auto a = fft(f, n);

    reverse(a.begin() + 1, a.end());
    for (int i = 0; i < n; i++) {
        a[i] /= n;
    }

    return a;
}

vector<int> multiply(const vector<int> &a, const vector<int> &b) {
    int n = 1;
    while (n < a.size() + b.size()) {
        n <<= 1;
    }

    vector<complex<double>> fa(a.begin(), a.end());
    vector<complex<double>> fb(b.begin(), b.end());
    fa.resize(n);
    fb.resize(n);

    fa = fft(fa, n);
    fb = fft(fb, n);

    vector<complex<double>> c(n);
    for (int i = 0; i < n; i++) {
        c[i] = fa[i] * fb[i];
    }

    c = rfft(c, n);

    vector<int> ans(a.size() + b.size() - 1);
    for (int i = 0; i < ans.size(); i++) {
        ans[i] = round(c[i].real());
    }

    return ans;
}

int main() {
    int n;
    cin >> n;

    vector<int> a(n + 1);
    for (int i = n; i >= 0; i--) {
        cin >> a[i];
    }

    int m;
    cin >> m;

    vector<int> b(m + 1);
    for (int i = m; i >= 0; i--) {
        cin >> b[i];
    }

    auto res = multiply(a, b);
    reverse(res.begin(), res.end());

    cout << res.size() - 1 << ' ';
    for (const auto &val : res) {
        cout << val << ' ';
    }
    cout << endl;
}

