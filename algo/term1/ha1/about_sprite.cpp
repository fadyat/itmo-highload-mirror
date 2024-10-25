#include <cmath>
#include <iostream>

using namespace std;

int main() {
    long long v;
    cin >> v;

    long long min_surface = 1e18;
    long long min_a, min_b, min_c;

    for (long long a = 1; a < (long long)std::pow(v, 1.0 / 3) + 10; a++) {
        if (v % a == 0) {
            for (long long b = 1; b < (long long)std::pow(v / a, 1.0 / 2) + 10; b++) {
                if ((v / a) % b == 0) {
                    long long c = v / a / b;
                    if (a * b * c == v) {
                        long long surface = 2 * (a * b + b * c + c * a);
                        if (surface < min_surface) {
                            min_surface = surface;
                            min_a = a;
                            min_b = b;
                            min_c = c;
                        }
                    }
                }
            }
        }
    }

    cout << min_a << " " << min_b << " " << min_c << endl;
}
