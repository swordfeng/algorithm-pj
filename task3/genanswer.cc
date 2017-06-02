#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    string s;
    ifstream fin(argv[1]);
    fin >> s;
    vector<string> ins(s.size()+1);
    string op;
    int x;
    char c;
    int cnt = 0;
    while (cin >> op) {
        if (op == "INS") {
            cin >> x >> c;
            ins[x].push_back(c);
            cnt++;
        } else if (op == "SUB") {
            cin >> x >> c;
            s[x] = c;
            cnt++;
        } else if (op == "DEL") {
            cin >> x;
            s[x] = '*';
            cnt++;
        }
    }
    for (int i = 0; i < s.size(); i++) {
        cout << ins[i];
        if (s[i] != '*') cout << s[i];
    }
    cout << ins[s.size()] << endl;
    cout << cnt << endl;
    return 0;
}
