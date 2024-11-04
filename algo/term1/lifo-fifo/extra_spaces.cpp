#include <iostream>

using namespace std;

string trim_spaces(string &s) {
    string result;
    bool is_space = false;

    for (char c : s) {
        if (isspace(c)) {
            if (!is_space && !result.empty()) {
                result += c;
            }
            is_space = true;
        } else {
            result += c;
            is_space = false;
        }
    }

    if (result.back() == ' ') {
        result.pop_back();
    }

    return result;
}

int main() {
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);

    string s;
    getline(cin, s);

    std::cout << trim_spaces(s) << std::endl;
}
