#include <iostream>
#include <vector>

using namespace std;

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int k, l;
    cin >> k >> l;

    vector<pair<char, string>> codes(k);
    for (int i = 0; i < codes.size(); ++i) {
        string letter, code;
        cin >> letter >> code;
        codes[i] = {letter[0], code};
    }

    string encoded;
    cin >> encoded;

    string decoded;
    for (int i = 0; i < encoded.size();) {
        for (int j = 0; j < codes.size(); ++j) {
            if (encoded.substr(i, codes[j].second.size()) == codes[j].second) {
                decoded += codes[j].first;
                i += codes[j].second.size();
                break;
            }
        }
    }

    cout << decoded << endl;
}
