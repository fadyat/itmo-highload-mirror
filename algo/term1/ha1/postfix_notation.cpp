#include <iostream>
#include <stack>

using namespace std;

int main() {
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);

    string s;
    stack<int> st;

    while (cin >> s) {
        if (s == "+") {
            int a = st.top();
            st.pop();
            int b = st.top();
            st.pop();
            st.push(a + b);
        } else if (s == "-") {
            int a = st.top();
            st.pop();
            int b = st.top();
            st.pop();
            st.push(b - a);
        } else if (s == "*") {
            int a = st.top();
            st.pop();
            int b = st.top();
            st.pop();
            st.push(a * b);
        } else {
            st.push(stoi(s));
        }
    }

    cout << st.top() << endl;
}
