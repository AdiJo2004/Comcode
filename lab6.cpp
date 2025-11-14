#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cctype>
using namespace std;

struct Production {
    vector<string> rhs;
    string lhs;
};

vector<string> tokenize(const string &str) {
    vector<string> tokens;
    string cur;
    auto flush = [&](){
        if (!cur.empty()) { tokens.push_back(cur); cur.clear(); }
    };
    for (char c : str) {
        if (isspace(static_cast<unsigned char>(c))) {
            flush();
        } else if (c=='(' || c==')' || c=='+' || c=='*' || c=='$') {
            flush();
            tokens.emplace_back(1, c);
        } else {
            cur.push_back(c);
        }
    }
    flush();
    return tokens;
}

string stackToString(stack<string> stk) {
    vector<string> elems;
    while (!stk.empty()) { elems.push_back(stk.top()); stk.pop(); }
    reverse(elems.begin(), elems.end());
    string out;
    for (auto &e : elems) out += e + " ";
    if (!out.empty()) out.pop_back();
    return out;
}

string inputBufferToString(const vector<string> &tokens, int ip) {
    string out;
    for (int i = ip; i < (int)tokens.size(); ++i) out += tokens[i] + " ";
    if (!out.empty()) out.pop_back();
    return out;
}

bool matchProduction(stack<string> &stk, const Production &prod) {
    int n = prod.rhs.size();
    if ((int)stk.size() < n) return false;
    vector<string> topElems(n);
    stack<string> temp = stk;
    for (int i = n - 1; i >= 0; --i) {
        topElems[i] = temp.top();
        temp.pop();
    }
    for (int i = 0; i < n; ++i)
        if (topElems[i] != prod.rhs[i]) return false;
    return true;
}

void doReduce(stack<string> &stk, const Production &prod) {
    int n = prod.rhs.size();
    for (int i = 0; i < n; ++i) stk.pop();
    stk.push(prod.lhs);
}

bool shouldReduce(const Production &prod, const string &lookahead) {
    // Delay E -> E + T if next is *
    if (prod.lhs == "E" && prod.rhs.size() == 3 &&
        prod.rhs[1] == "+" && lookahead == "*") return false;
    // Delay E -> T if next is *
    if (prod.lhs == "E" && prod.rhs.size() == 1 &&
        prod.rhs[0] == "T" && lookahead == "*") return false;
    return true;
}

int main() {
    vector<Production> productions;
    int n;

    cout << "Enter number of productions in the grammar: ";
    cin >> n;
    cin.ignore();

    cout << "Enter productions (LHS -> RHS, space-separated RHS symbols):\n";
    for (int i = 0; i < n; ) {
        string line; getline(cin, line);
        if (line.empty()) continue;
        size_t arrowPos = line.find("->");
        if (arrowPos == string::npos) {
            cout << "Invalid format. Use LHS -> RHS\n";
            continue;
        }
        string lhs = line.substr(0, arrowPos);
        lhs.erase(remove(lhs.begin(), lhs.end(), ' '), lhs.end());
        string rhsStr = line.substr(arrowPos + 2);
        vector<string> rhs = tokenize(rhsStr);
        if (lhs.empty() || rhs.empty()) {
            cout << "Empty LHS or RHS not allowed\n";
            continue;
        }
        productions.push_back({rhs, lhs});
        ++i;
    }

    cout << "\nEnter input strings to parse (e.g., id+id*id or with spaces). Enter '0' to quit.\n";

    while (true) {
        cout << "\nInput string (Enter 0 to exit): ";
        string raw; getline(cin, raw);
        if (raw == "0") break;

        vector<string> tokens = tokenize(raw);
        tokens.push_back("$");

        stack<string> stk;
        stk.push("$");
        int ip = 0;

        cout << left << setw(25) << "Stack" << setw(25) << "Input Buffer" << "Action\n";
        cout << string(65, '-') << "\n";

        bool accepted = false;

        while (true) {
            string stackStr = stackToString(stk);
            string inputBufStr = inputBufferToString(tokens, ip);
            string actionStr;

            // Accept
            if (stk.size() == 2 && stk.top() == "E" && tokens[ip] == "$") {
                actionStr = "Accept";
                cout << left << setw(25) << stackStr << setw(25) << inputBufStr << actionStr << "\n";
                accepted = true;
                break;
            }

            // Try reductions repeatedly until no match
            bool reduced = false;
            while (true) {
                bool didReduce = false;
                for (auto &prod : productions) {
                    if (matchProduction(stk, prod) && shouldReduce(prod, tokens[ip])) {
                        actionStr = "Reduce by: " + prod.lhs + " ->";
                        for (auto &s : prod.rhs) actionStr += " " + s;
                        cout << left << setw(25) << stackStr << setw(25) << inputBufStr << actionStr << "\n";
                        doReduce(stk, prod);
                        didReduce = true;
                        stackStr = stackToString(stk); // update for next log
                    }
                }
                if (!didReduce) break;
                reduced = true;
            }

            if (reduced) continue;

            // Shift
            if (tokens[ip] != "$") {
                actionStr = "Shift " + tokens[ip];
                cout << left << setw(25) << stackStr << setw(25) << inputBufStr << actionStr << "\n";
                stk.push(tokens[ip]);
                ++ip;
            } else {
                actionStr = "Error: Unable to parse";
                cout << left << setw(25) << stackStr << setw(25) << inputBufStr << actionStr << "\n";
                break;
            }
        }

        if (!accepted) cout << "Parsing failed.\n";
    }
    return 0;
}


