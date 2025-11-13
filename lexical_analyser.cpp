#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>
#include <cctype>
#include <vector>
using namespace std;

// Token sets
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

unordered_set<char> special_symbols = {
    '(', ')', '{', '}', ';', ','
};

// Token structure
struct Token {
    string type;
    string value;
    int line;
};

vector<Token> tokens;
vector<string> error_messages;
int current_line = 1;

// Helper functions
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
    size_t start = 0;
    bool dot = false;
    
    for (size_t i = start; i < word.size(); i++) {
        if (word[i] == '.') {
            if (dot) return false;
            dot = true;
        } else if (!isdigit(word[i])) {
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

    string word;
    char ch;

    while (file.get(ch)) {
        // Track line numbers
        if (ch == '\n') current_line++;

        // Skip whitespace
        if (isspace(ch)) continue;

        // Handle comments
        if (ch == '/') {
            if (file.peek() == '/') {
                while (file.get(ch) && ch != '\n') current_line += (ch == '\n');
                continue;
            }
            else if (file.peek() == '*') {
                file.get();
                while (file.get(ch)) {
                    if (ch == '\n') current_line++;
                    if (ch == '*' && file.peek() == '/') {
                        file.get();
                        break;
                    }
                }
                continue;
            }
        }

        // Handle string literals
        if (ch == '"') {
            string literal = "\"";
            int start_line = current_line;
            while (file.get(ch) && ch != '"') {
                if (ch == '\n') current_line++;
                literal += ch;
            }
            if (ch == '"') {
                literal += '"';
                tokens.push_back({"<literal>", literal, start_line});
            } else {
                error_messages.push_back("Error at line " + to_string(start_line) + 
                                      ": Unterminated string literal");
            }
            continue;
        }

        // Handle character literals
        if (ch == '\'') {
            string literal = "'";
            int start_line = current_line;
            file.get(ch);
            literal += ch;
            if (file.get() == '\'') {
                literal += "'";
                tokens.push_back({"<literal>", literal, start_line});
            } else {
                error_messages.push_back("Error at line " + to_string(start_line) + 
                                      ": Unterminated character literal");
            }
            continue;
        }

        // Handle words (identifiers/keywords/numbers)
        if (isalpha(ch) || ch == '_' || isdigit(ch)) {
            word = ch;
            int start_line = current_line;
            while (file.peek() && (isalnum(file.peek()) || file.peek() == '_' || file.peek() == '.')) {
                file.get(ch);
                if (ch == '\n') current_line++;
                word += ch;
            }
            
            if (isKeyword(word)) {
                tokens.push_back({"<keyword>", word, start_line});
            } else if (isNumber(word)) {
                tokens.push_back({"<num>", word, start_line});
            } else if (isValidIdentifier(word)) {
                tokens.push_back({"<id>", word, start_line});
            } else {
                error_messages.push_back("Error at line " + to_string(start_line) + 
                                      ": Invalid token '" + word + "'");
            }
            continue;
        }

        // Handle operators
        if (isOperator(string(1, ch))) {
            string op(1, ch);
            if (isOperator(string(1, file.peek()))) {
                string potentialOp = op + string(1, file.peek());
                if (isOperator(potentialOp)) {
                    file.get();
                    op = potentialOp;
                }
            }
            tokens.push_back({"<op>", op, current_line});
            continue;
        }

        // Handle special symbols
        if (isSpecialSymbol(ch)) {
            tokens.push_back({"<special symbol>", string(1, ch), current_line});
            continue;
        }

        // Generate error message for unrecognized symbols
        error_messages.push_back("Error at line " + to_string(current_line) + 
                              ": Unrecognized symbol '" + string(1, ch) + "'");
    }

    file.close();
}

void displayResults() {
    cout << "\n--- Token Listing ---\n";
    for (const auto& token : tokens) {
        cout << "[Line " << token.line << "] " << token.type << " : " << token.value << endl;
    }

    // Display error messages
    if (!error_messages.empty()) {
        cout << "\n--- Errors ---\n";
        for (const auto& msg : error_messages) {
            cout << msg << endl;
        }
    }

    // Count statistics
    int keywordCount = 0, idCount = 0, numCount = 0;
    int opCount = 0, specialCount = 0, literalCount = 0;

    for (const auto& token : tokens) {
        if (token.type == "<keyword>") keywordCount++;
        else if (token.type == "<id>") idCount++;
        else if (token.type == "<num>") numCount++;
        else if (token.type == "<op>") opCount++;
        else if (token.type == "<special symbol>") specialCount++;
        else if (token.type == "<literal>") literalCount++;
    }

    cout << "\n--- Token Summary ---\n";
    cout << "Total <keyword> tokens: " << keywordCount << endl;
    cout << "Total <id> tokens: " << idCount << endl;
    cout << "Total <num> tokens: " << numCount << endl;
    cout << "Total <op> tokens: " << opCount << endl;
    cout << "Total <special symbol> tokens: " << specialCount << endl;
    cout << "Total <literal> tokens: " << literalCount << endl;
    cout << "Total errors: " << error_messages.size() << endl;
}

int main() {
    string filename;
    filename="hello.cpp";

    processFile(filename);
    displayResults();

    return 0;
}
