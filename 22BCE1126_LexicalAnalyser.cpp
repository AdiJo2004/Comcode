#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>
#include <cctype>
using namespace std;

unordered_set<string> keywords = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "float", "for", "goto", "if", "int", "long",
    "register", "return", "short", "signed", "sizeof", "static", "struct",
    "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
};

unordered_set<string> operators = {
    "+", "-", "*", "/", "%", "++", "--", "==", "!=", "<", "<=", ">", ">=",
    "&&", "||", "!", "=", "+=", "-=", "*=", "/=", "%=", "&", "|", "^", "~", "<<", ">>"
};

unordered_set<char> special_symbols = {'(', ')', '{', '}', ';', ','};

bool isKeyword(const string& word);
bool isOperator(const string& op);
bool isSpecialSymbol(char ch);
bool isValidIdentifier(const string& word);
bool isNumber(const string& word);
void processFile(const string& filename);

int keywordCount = 0, identifierCount = 0, numberCount = 0;
int operatorCount = 0, specialCount = 0, literalCount = 0, errorCount = 0;

int main() {
    string filename;
    filename="hello.cpp";
    
    processFile(filename);
    
    cout << "\n--- Token Counts ---\n";
    cout << "Keywords: " << keywordCount << endl;
    cout << "Identifiers: " << identifierCount << endl;
    cout << "Numbers: " << numberCount << endl;
    cout << "Operators: " << operatorCount << endl;
    cout << "Special Symbols: " << specialCount << endl;
    cout << "Literals: " << literalCount << endl;
    cout << "Errors: " << errorCount << endl;
    
    return 0;
}

bool isKeyword(const string& word) {
    return keywords.find(word) != keywords.end();
}

bool isOperator(const string& op) {
    return operators.find(op) != operators.end();
}

bool isSpecialSymbol(char ch) {
    return special_symbols.find(ch) != special_symbols.end();
}

bool isValidIdentifier(const string& word) {
    if (word.empty() || !(isalpha(word[0]) || word[0] == '_'))
        return false;
    for (char ch : word) {
        if (!isalnum(ch) && ch != '_')
            return false;
    }
    return true;
}

bool isNumber(const string& word) {
    if (word.empty()) return false;
    bool dot = false;
    for (char ch : word) {
        if (ch == '.') {
            if (dot) return false;
            dot = true;
        } else if (!isdigit(ch)) {
            return false;
        }
    }
    return true;
}

void processFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file.\n";
        return;
    }

    string token;
    char ch;
    int current_line = 1;

    while (file.get(ch)) {
        if (ch == '\n') current_line++;

        if (isspace(ch)) continue;

        if (ch == '/') {
            if (file.peek() == '/') {
                while (file.get(ch) && ch != '\n');
                continue;
            }
            else if (file.peek() == '*') {
                file.get();
                while (file.get(ch)) {
                    if (ch == '*' && file.peek() == '/') {
                        file.get();
                        break;
                    }
                }
                continue;
            }
        }

        if (ch == '"') {
            string literal = "\"";
            int start_line = current_line;
            while (file.get(ch) && ch != '"') {
                literal += ch;
            }
            if (ch == '"') {
                literal += '"';
                cout << "[Line " << start_line << "] literal: " << literal << endl;
                literalCount++;
            } else {
                cout << "[Line " << start_line << "] ERROR: Unterminated string\n";
                errorCount++;
            }
            continue;
        }

        if (isalpha(ch) || ch == '_' || isdigit(ch)) {
            token = ch;
            while (file.peek() && (isalnum(file.peek()) || file.peek() == '_' || file.peek() == '.')) {
                file.get(ch);
                token += ch;
            }
            
            if (isKeyword(token)) {
                cout << "[Line " << current_line << "] keyword: " << token << endl;
                keywordCount++;
            } else if (isNumber(token)) {
                cout << "[Line " << current_line << "] number: " << token << endl;
                numberCount++;
            } else if (isValidIdentifier(token)) {
                cout << "[Line " << current_line << "] identifier: " << token << endl;
                identifierCount++;
            } else {
                cout << "[Line " << current_line << "] ERROR: Invalid token '" << token << "'\n";
                errorCount++;
            }
            continue;
        }

        if (isOperator(string(1, ch))) {
            string op(1, ch);
            if (isOperator(string(1, file.peek()))) {
                string potentialOp = op + string(1, file.peek());
                if (isOperator(potentialOp)) {
                    file.get();
                    op = potentialOp;
                }
            }
            cout << "[Line " << current_line << "] operator: " << op << endl;
            operatorCount++;
            continue;
        }
        
        if (isSpecialSymbol(ch)) {
            cout << "[Line " << current_line << "] special symbol: " << ch << endl;
            specialCount++;
            continue;
        }

        cout << "[Line " << current_line << "] ERROR: Unrecognized symbol '" << ch << "'\n";
        errorCount++;
    }

    file.close();
}

