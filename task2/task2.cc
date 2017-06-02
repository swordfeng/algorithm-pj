#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

using namespace std;

enum OP {
    INS = 0,
    SUB = 1,
    DEL = 2
};

const int NODELEN = 1000000;

struct Log;
extern const shared_ptr<Log> nil;

struct Log {
    OP op;
    int x;
    char c;
    shared_ptr<Log> last;

    Log(): x(0), c(0), last(nil) {}
    Log(OP op, int x, char c, shared_ptr<Log> last): 
        op(op), x(x), c(c), last(last) {}
    void print();
    void doo(vector<string> &list);
};
const shared_ptr<Log> nil = make_shared<Log>();
const Log *LOG_NIL = nil.get();

void Log::print() {
    if (this != LOG_NIL) {
        last->print();
        switch (op) {
        case INS:
            printf("INS %d %c\n", x, c);
            break;
        case SUB:
            printf("SUB %d %c\n", x, c);
            break;
        case DEL:
            printf("DEL %d\n", x);
            break;
        }
    }
}
void Log::doo(vector<string> &list) {
    if (this == LOG_NIL) return;
    string &orig = list[x];
    switch (op) {
    case INS:
        orig = string(1, c) + orig;
        break;
    case SUB:
        orig[orig.size()-1] = c;
        break;
    case DEL:
        orig.pop_back();
        break;
    }
    last->doo(list);
}
vector<string> conv(string s) {
    vector<string> r(s.size()+1);
    for (int i = 0; i < s.size(); i++) r[i] = string(1, s[i]);
    return r;
}
string unconv(vector<string> r) {
    string s;
    for (string &ss : r) s += ss;
    return s;
}


shared_ptr<Log> ins(int x, char c, shared_ptr<Log> last) {
    return make_shared<Log>(INS, x, c, last);
}
shared_ptr<Log> sub(int x, char c, shared_ptr<Log> last) {
    return make_shared<Log>(SUB, x, c, last);
}
shared_ptr<Log> del(int x, shared_ptr<Log> last) {
    return make_shared<Log>(DEL, x, 0, last);
}

struct PreWorkState {
    shared_ptr<Log> log;
    int times;
};
void updatePreWork(int j, int i);

struct Node {
    shared_ptr<Log> log;
    int times;
    // prework
    vector<PreWorkState> preWork;
};

string a;
vector<int> graph[NODELEN], prevgraph[NODELEN];
string s[NODELEN];
int n;
int k;

Node nodes[NODELEN], newnodes[NODELEN];

int doWork() {
    vector<int> nodeseq(n), nodeseqNext, pending, pendingNext;
    nodeseqNext.reserve(n);
    pending.reserve(n);
    pendingNext.reserve(n);
    vector<bool> marked(n);
    // init
    for (int j = 0; j < n; j++) {
        nodeseq[j] = j;
        nodes[j].preWork.resize(k + 1);
        updatePreWork(j, 0);
        nodes[j].times = nodes[j].preWork[k].times;
        nodes[j].log = nodes[j].preWork[k].log;
        newnodes[j].times = -1;
    }
    // work
    for (int i = 1; i <= a.size(); i++) {
        if (i % 100 == 0) fprintf(stderr, "%d\n", i);
        int pn = nodes[nodeseq[0]].times;
        for (int j : nodeseq) {
            // flush queue
            int p = nodes[j].times;
            if (p > pn + 1) {
                nodeseqNext.insert(nodeseqNext.end(), pending.begin(), pending.end());
                pending.resize(0);
                nodeseqNext.insert(nodeseqNext.end(), pendingNext.begin(), pendingNext.end());
                pendingNext.resize(0);
            } else if (p == pn + 1) {
                nodeseqNext.insert(nodeseqNext.end(), pending.begin(), pending.end());
                swap(pending, pendingNext);
                pendingNext.resize(0);
            }
            pn = p;
            // prework
            updatePreWork(j, i);
            if (nodes[j].preWork.size() > 0) {
                if (newnodes[j].times == -1 || nodes[j].preWork[k].times < newnodes[j].times) {
                    newnodes[j].times = nodes[j].preWork[k].times;
                    newnodes[j].log = nodes[j].preWork[k].log;
                }
                if (nodes[j].preWork[k].times == i - k) {
                    // must DEL (i-k) times and this is the real time, so all ops are DEL
                    // then we confirm that more operations are all DEL, so we do not need them
                    nodes[j].preWork.resize(0);
                    nodes[j].preWork.shrink_to_fit();
                }
            }
            // del
            if (newnodes[j].times == -1 || nodes[j].times + 1 <= newnodes[j].times) {
                newnodes[j].times = nodes[j].times + 1;
                newnodes[j].log = del(i-1, nodes[j].log);
            }
            // same
            if (a[i-1] == s[j][k-1]) {
                for (int l : prevgraph[j]) {
                    if (nodes[l].times < newnodes[j].times) {
                        newnodes[j].times = nodes[l].times;
                        newnodes[j].log = nodes[l].log;
                    }
                }
            } else { // sub
                for (int l : prevgraph[j]) {
                    if (nodes[l].times + 1 < newnodes[j].times) {
                        newnodes[j].times = nodes[l].times + 1;
                        newnodes[j].log = sub(i-1, s[j][k-1], nodes[l].log);
                    }
                }
            }
            // add node back to seq
            if (newnodes[j].times < p) { // max is (p-1), or it must be incorrect
                nodeseqNext.push_back(j);
            } else if (newnodes[j].times == p) {
                pending.push_back(j);
            } else { // newnodes[j].times == p + 1
                pendingNext.push_back(j);
            }
        }
        // save back!
        nodeseq.resize(0);
        nodeseq.insert(nodeseq.end(), nodeseqNext.begin(), nodeseqNext.end());
        nodeseq.insert(nodeseq.end(), pending.begin(), pending.end());
        nodeseq.insert(nodeseq.end(), pendingNext.begin(), pendingNext.end());
        nodeseqNext.resize(0);
        pending.resize(0);
        pendingNext.resize(0);
        int pseq = 0;
        auto pend = pending.begin();
        for (int j = 0; j < n; j++) marked[j] = false;
        while (pseq < n) {
            while (pseq < n && marked[nodeseq[pseq]]) pseq++;
            if (pseq == n) break;
            int j = nodeseq[pseq];
            if (pend != pending.end() && newnodes[*pend].times < newnodes[j].times) {
                j = *pend++;
            } else {
                pseq++;
            }
            nodeseqNext.push_back(j);
            // ins
            // it must be correct beacuse all nodes that has less than p dist has already been processed
            for (int g : graph[j]) {
                if (newnodes[j].times + 1 < newnodes[g].times) {
                    newnodes[g].times = newnodes[j].times + 1;
                    newnodes[g].log = ins(i, s[g][k-1], newnodes[j].log);
                    pending.push_back(g);
                    marked[g] = true;
                }
            }
        }
        // save back!
        nodeseq.resize(0);
        nodeseq.insert(nodeseq.end(), nodeseqNext.begin(), nodeseqNext.end());
        nodeseq.insert(nodeseq.end(), pend, pending.end());
        nodeseqNext.resize(0);
        pending.resize(0);
        for (int j = 0; j < n; j++) {
            nodes[j].times = newnodes[j].times;
            nodes[j].log = newnodes[j].log;
            newnodes[j].times = -1;
            newnodes[j].log = nil;
        }
    }
    return nodeseq[0];
}

void updatePreWork(int j, int i) {
    if (nodes[j].preWork.size() == 0) {
        return;
    }
    auto &preWork = nodes[j].preWork;
    if (i == 0) {
        preWork[0].times = 0;
        preWork[0].log = nil;
        for (int t = 1; t <= k; t++) {
            preWork[t].times = preWork[t-1].times + 1;
            preWork[t].log = ins(0, s[j][t-1], preWork[t-1].log);
        }
    } else {
        for (int t = k; t > 0; t--) {
            if (a[i-1] == s[j][t-1]) {
                preWork[t].times = preWork[t-1].times;
                preWork[t].log = preWork[t-1].log;
            } else if (preWork[t-1].times <= preWork[t].times) {
                preWork[t].times = preWork[t-1].times + 1;
                preWork[t].log = sub(i-1, s[j][t-1], preWork[t-1].log);
            } else {
                preWork[t].times++;
                preWork[t].log = del(i-1, preWork[t].log);
            }
        }
        preWork[0].times++;
        preWork[0].log = del(i-1, preWork[0].log);
        for (int t = 1; t <= k; t++) {
            if (preWork[t-1].times + 1 <= preWork[t].times) {
                preWork[t].times = preWork[t-1].times + 1;
                preWork[t].log = ins(i, s[j][t-1], preWork[t-1].log);
            }
        }
    }
}

void addEdge(int i, int j);
void makeGraph();
int main() {
    cin >> a >> n;
    for (int i = 0; i < n; i++) {
        cin >> s[i];
    }
    k = s[0].size();
    makeGraph();
    int j = doWork();
    auto list = conv(a);
    nodes[j].log->doo(list);
    printf("%s\n", unconv(list).c_str());
    printf("%d\n", nodes[j].times);
    nodes[j].log->print();
    return 0;
}

void makeGraph() {
    unordered_map<string, vector<int>> t;
    for (int i = 0; i < n; i++) {
        string key = s[i].substr(0, k-1);
        t[key].push_back(i);
    }
    for (int i = 0; i < n; i++) {
        string key = s[i].substr(1);
        for (int j : t[key]) {
            addEdge(i, j);
        }
    }
}

void addEdge(int i, int j) {
    graph[i].push_back(j);
    prevgraph[j].push_back(i);
}

