#include <bits/stdc++.h>
using namespace std;

unordered_set<string> keywords = {"int", "float", "double", "long", "return", "void", "if", "else", "while", "for"};
unordered_set<char> singleOp = {'=', '+', '-', '*', '/', '<', '>', '!', '%'};
unordered_set<string> multiOp = {"==","!=","<=",">=","++","--","+=","-=","*=","/="};
unordered_set<char> special = {')','(','{','}',';',','};

struct SymbolEntry {
    int entryNo;
    string lexeme;
    string tokenType;
    int lineDeclared;
    vector<int> lineUsed;
};

vector<SymbolEntry> symbolTable;

bool isKeyword(string s) {
    return keywords.find(s) != keywords.end();
}

bool isIdentifierStart(char c) {
    return isalpha(c) || c == '_';
}
bool isIdentifierChar(char c) {
    return isalnum(c) || c == '_';
}

void addToSymbolTable(string lexeme, string type, int line) {
    // check for existing entry
    for (auto &e : symbolTable) {
        if (e.lexeme == lexeme && e.tokenType == type) {
            e.lineUsed.push_back(line);
            return;
        }
    }
    SymbolEntry newEntry;
    newEntry.entryNo = symbolTable.size() + 1;
    newEntry.lexeme = lexeme;
    newEntry.tokenType = type;
    newEntry.lineDeclared = line;
    newEntry.lineUsed.push_back(line);
    symbolTable.push_back(newEntry);
}

void process(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file\n";
        return;
    }

    string line;
    int lineNo = 0;
    bool inMultiComment = false;

    while (getline(file, line)) {
        lineNo++;
        int i = 0, len = line.length();

        while (i < len) {
            char c = line[i];

            if (isspace(c)) { i++; continue; }

            if (c == '/' && i+1 < len && line[i+1] == '/') break;

            if (c == '/' && i+1 < len && line[i+1] == '*') {
                inMultiComment = true;
                i += 2;
                continue;
            }

            if (inMultiComment) {
                if (c == '*' && i+1 < len && line[i+1] == '/') {
                    inMultiComment = false;
                    i += 2;
                } else i++;
                continue;
            }

            if (c == '"') {
                string literal = "";
                i++; 
                while (i < len && line[i] != '"') {
                    literal += line[i++];
                }
                if (i < len && line[i] == '"') {
                    i++;
                    cout << "Literal: \"" << literal << "\"\n";
                    addToSymbolTable("\"" + literal + "\"", "Literal", lineNo);
                } else {
                    cout << "Lexical Error: Unterminated string literal at line " << lineNo << endl;
                }
                continue;
            }

            if (isIdentifierStart(c)) {
                string token = "";
                while (i < len && isIdentifierChar(line[i])) {
                    token += line[i++];
                }
                if (isKeyword(token)) {
                    cout << "Keyword: " << token << endl;
                } else {
                    cout << "Identifier: " << token << endl;
                    addToSymbolTable(token, "Identifier", lineNo);
                }
                continue;
            }

            if (isdigit(c)) {
                string number = "";
                bool isFloat = false;

                while (i < len && (isdigit(line[i]) || line[i] == '.')) {
                    if (line[i] == '.') {
                        if (isFloat) break;
                        isFloat = true;
                    }
                    number += line[i++];
                }
                if (isFloat) {
                    cout << "Float: " << number << endl;
                    addToSymbolTable(number, "Float", lineNo);
                } else {
                    cout << "Integer: " << number << endl;
                    addToSymbolTable(number, "Integer", lineNo);
                }
                continue;
            }

            if (i+1 < len) {
                string op = string(1, c) + line[i+1];
                if (multiOp.find(op) != multiOp.end()) {
                    cout << "Operator: " << op << endl;
                    i += 2;
                    continue;
                }
            }

        
            if (singleOp.find(c) != singleOp.end()) {
                cout << "Operator: " << c << endl;
                i++;
                continue;
            }

            if (special.find(c) != special.end()) {
                cout << "Special Symbol: " << c << endl;
                i++;
                continue;
            }

            cout << "Lexical Error: Unrecognized symbol '" << c << "' at line " << lineNo << endl;
            i++;
        }
    }

    file.close();
    
    cout << "\n===== SYMBOL TABLE =====\n";
    cout << "Entry\tLexeme\t\tToken Type\tDeclared\tUsed Lines\n";
    for (auto &e : symbolTable) {
        cout << e.entryNo << "\t" << e.lexeme << "\t\t" << e.tokenType << "\t\t"
             << e.lineDeclared << "\t\t";
        for (int ln : e.lineUsed) cout << ln << " ";
        cout << endl;
    }
}
int main() {
    string name;
    cout << "Enter file name: ";
    cin >> name;
    process(name);
}




