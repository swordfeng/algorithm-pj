#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

enum OP {
    INS = 0,
    SUB = 1,
    DEL = 2
};

const int NODELEN = 10000;

struct Log {
    OP op;
    int x;
    char c;
    shared_ptr<Log> last;

    void print();
    void doo(vector<string> &list);
};
Log log_nil;
const Log *LOG_NIL = &log_nil;

void Log::print() {
    if (this != LOG_NIL) {
        last.print();
        switch op {
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
    switch op {
    case INS:
        list[x] = string(1, c) + list[x];
        break;
    case SUB:
        string &orig = list[x];
        orig[orig.size()-1] = c;
        break;
    case DEL:
        string &orig := list[x];
        orig.pop_back();
        break;
    }
    last.doo(list)
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
    return make_shared<Log>({INS, x, c, last});
}
shared_ptr<Log> sub(int x, char c, shared_ptr<Log> last) {
    return make_shared<Log>({SUB, x, c, last});
}
shared_ptr<Log> del(int x, shared_ptr<Log> last) {
    return make_shared<Log>({DEL, x, 0, last});
}

struct PreWorkState {
    shared_ptr<Log> log;
    int times;
}

struct Node {
    shared_ptr<Log> log;
    int times;
    // prework
    vector<PreWorkState> preWork;
}

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
        updatePreWork(j, 0)
        nodes[j].times = nodes[j].preWork[k].times;
        nodes[j].log = nodes[j].preWork[k].log;
        newnodes[j].times = -1;
    }
    // work
    for (int i = 1; i <= a.size(); i++) {
        if (i % 100 == 0) {
            fprintf(stderr, "%d\n", i);
        }
        int pn = nodes[nodeseq[0]].times;
        for (int j : nodeseq) {
//=================================================================================
            // flush queue
            p := nodes[j].times
            if p > pn + 1 {
                nodeseqNext = append(nodeseqNext, pending...)
                pending = pending[:0]
                nodeseqNext = append(nodeseqNext, pendingNext...)
                pendingNext = pendingNext[:0]
            } else if p == pn + 1 {
                nodeseqNext = append(nodeseqNext, pending...)
                pending, pendingNext = pendingNext, pending[:0]
            }
            pn = p
            // prework
            updatePreWork(j, i)
            if nodes[j].preWork != nil {
                if newnodes[j].times == -1 || nodes[j].preWork[k].times < newnodes[j].times {
                    newnodes[j].times = nodes[j].preWork[k].times
                    newnodes[j].log = nodes[j].preWork[k].log
                }
                if nodes[j].preWork[k].times == i - k {
                    // must DEL (i-k) times and this is the real time, so all ops are DEL
                    // then we confirm that more operations are all DEL, so we do not need them
                    nodes[j].preWork = nil
                }
            }
            // del
            if newnodes[j].times == -1 || nodes[j].times + 1 <= newnodes[j].times {
                newnodes[j].times = nodes[j].times + 1
                newnodes[j].log = del(i-1, nodes[j].log)
            }
            // same
            if a[i-1] == s[j][k-1] {
                for _, l := range prevgraph[j] {
                    if nodes[l].times < newnodes[j].times {
                        newnodes[j].times = nodes[l].times
                        newnodes[j].log = nodes[l].log
                    }
                }
            } else { // sub
                for _, l := range prevgraph[j] {
                    if nodes[l].times + 1 < newnodes[j].times {
                        newnodes[j].times = nodes[l].times + 1
                        newnodes[j].log = sub(i-1, s[j][k-1], nodes[l].log)
                    }
                }
            }
            // add node back to seq
            if newnodes[j].times < p { // max is (p-1), or it must be incorrect
                nodeseqNext = append(nodeseqNext, j)
            } else if newnodes[j].times == p {
                pending = append(pending, j)
            } else { // newnodes[j].times == p + 1
                pendingNext = append(pendingNext, j)
            }
        }
        // save back!
        nodeseqNext = append(nodeseqNext, pending...)
        nodeseqNext = append(nodeseqNext, pendingNext...)
        nodeseq, nodeseqNext, pending, pendingNext = nodeseqNext, nodeseq[:0], pending[:0], pendingNext[:0]
        pseq := 0
        pend := pending
        for j := 0; j < n; j++ {marked[j] = false}
        for pseq < n {
            for pseq < n && marked[nodeseq[pseq]] { pseq++ }
            if pseq == n { break }
            j := nodeseq[pseq]
            if len(pend) > 0 && newnodes[pend[0]].times < newnodes[j].times {
                j = pend[0]
                pend = pend[1:]
            } else {
                pseq++
            }
            nodeseqNext = append(nodeseqNext, j)
            // ins
            // it must be correct beacuse all nodes that has less than p dist has already been processed
            for _, g := range graph[j] {
                if newnodes[j].times + 1 < newnodes[g].times {
                    newnodes[g].times = newnodes[j].times + 1
                    newnodes[g].log = ins(i, s[g][k-1], newnodes[j].log)
                    pend = append(pend, g)
                    marked[g] = true
                }
            }
        }
        // save back!
        nodeseqNext = append(nodeseqNext, pend...)
        nodeseq, nodeseqNext, pending, pendingNext = nodeseqNext, nodeseq[:0], pending[:0], pendingNext[:0]
        for j := 0; j < n; j++ {
            nodes[j].times = newnodes[j].times
            nodes[j].log = newnodes[j].log
            newnodes[j].times = -1
            newnodes[j].log = nil
        }
    }
    return nodeseq[0]
}

func updatePreWork(j, i int) {
    if nodes[j].preWork == nil {
        return
    }
    preWork := nodes[j].preWork
    if i == 0 {
        preWork[0].times = 0
        preWork[0].log = nil
        for t := 1; t <= k; t++ {
            preWork[t].times = preWork[t-1].times + 1
            preWork[t].log = ins(0, s[j][t-1], preWork[t-1].log)
        }
    } else {
        for t := k; t > 0; t-- {
            if a[i-1] == s[j][t-1] {
                preWork[t].times = preWork[t-1].times
                preWork[t].log = preWork[t-1].log
            } else if preWork[t-1].times <= preWork[t].times {
                preWork[t].times = preWork[t-1].times + 1
                preWork[t].log = sub(i-1, s[j][t-1], preWork[t-1].log)
            } else {
                preWork[t].times++
                preWork[t].log = del(i-1, preWork[t].log)
            }
        }
        preWork[0].times++
        preWork[0].log = del(i-1, preWork[0].log)
        for t := 1; t <= k; t++ {
            if preWork[t-1].times + 1 <= preWork[t].times {
                preWork[t].times = preWork[t-1].times + 1
                preWork[t].log = ins(i, s[j][t-1], preWork[t-1].log)
            }
        }
    }
}

func main() {
    Scanln(&a)
    Scanln(&n)
    for i := 0; i < n; i++ {
        Scanln(&s[i])
    }
    k = len(s[0])
    makeGraph()
    j := doWork()
    list := conv(a)
    nodes[j].log.Do(list)
    Println(unconv(list))
    Printf("%d\n", nodes[j].times)
    nodes[j].log.Print()
}

func makeGraph() {
    var t = make(map[string][]int)
    for i := 0; i < n; i++ {
        key := s[i][:k-1]
        t[key] = append(t[key], i)
    }
    for i := 0; i < n; i++ {
        key := s[i][1:]
        for _, j := range t[key] {
            addEdge(i, j)
        }
    }
}

func addEdge(i, j int) {
    graph[i] = append(graph[i], j)
    prevgraph[j] = append(prevgraph[j], i)
}

