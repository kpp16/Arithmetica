#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <stack>
#include <string>

enum Token {
    tok_eof = -2,
    tok_endline = -7,

    // Commands
    tok_print = -3,

    //primary
    tok_identifier = -4,
    tok_number = -5,
    tok_operator = -6
};

std::string IdentifierStr = "";
double NumVal;
std::string Operator;

std::unordered_map<std::string, double> data;

/**
 * @brief Returns the next token from the input
 * 
 * @return int The token number
 */
static int gettok() {
    static int LastChar = ' ';

    // Skip any whitespace.
    while (isspace(LastChar))
        LastChar = getchar();

    if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
        IdentifierStr = "";
        IdentifierStr += LastChar;
        while (isalnum((LastChar = getchar())))
            IdentifierStr += LastChar;

        if (IdentifierStr == "print")
            return tok_print;
        return tok_identifier;
    }

    if (isdigit(LastChar) || LastChar == '.') { // Number: [0-9.]+
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar) || LastChar == '.');

        NumVal = strtod(NumStr.c_str(), nullptr);
        return tok_number;
    }

    if (LastChar == '#') {
    // Comment until end of line.
        do
            LastChar = getchar();
        while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF)
            return gettok();
    }

    if (LastChar == '+' || LastChar == '-' || LastChar == '*' || LastChar == '/' ||
        LastChar == '=' || LastChar == '>' || LastChar == '<' || LastChar == '|' ||
        LastChar == '&' || LastChar == '(' || LastChar == ')') {

            Operator = "";
            Operator += LastChar;
            LastChar = getchar();
            return tok_operator;
    }

    if (LastChar == ';') {
        LastChar = getchar();
        return tok_endline;
    }

    // Check for end of file.  Don't eat the EOF.
    if (LastChar == EOF)
        return tok_eof;

    // Otherwise, just return the character as its ascii value.
    int ThisChar = LastChar;
    LastChar = getchar();
    return ThisChar;
}

static int CurTok;
static int getNextToken() {
    return CurTok = gettok();
}

static std::map<char, int> BinopPrecedence; // Holds the Precedence of each operator

static int GetTokPrecedence() {
  if (!isascii(CurTok))
    return -1;

  // Make sure it's a declared binop.
  int TokPrec = BinopPrecedence[CurTok];
  if (TokPrec <= 0)
    return -1;
  return TokPrec;
}

int LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return -1;
}

int precedence(char op) {
    if(op == '+'||op == '-')
        return 1;
    if(op == '*'||op == '/')
        return 2;
    return 0;
}

double applyOp(double a, double b, char op) {
    switch(op){
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
    }
}

bool invalidSyntax = false;

double EvaluateRHS() {
    if (CurTok == tok_endline) {
        LogError("Invalid Syntax");
        getNextToken();
        invalidSyntax = true;
        return -1;
    }
    std::vector<std::string> tokens;
    
    while (CurTok != tok_endline) {
        if (CurTok == tok_number) {
            tokens.push_back(std::to_string(NumVal));
            getNextToken();
        } else if (CurTok == tok_identifier) {
            if (data.find(IdentifierStr) == data.end()) {
                LogError("Identifier not found");
                invalidSyntax = true;
                getNextToken();
                return -1;
            } else {
                double val = data[IdentifierStr];
                tokens.push_back(std::to_string(val));
                getNextToken();
            }
        } else if (CurTok == tok_operator) {
            tokens.push_back(Operator);
            getNextToken();
        } else {
            LogError("Invalid Syntax");
            invalidSyntax = true;
            getNextToken();
            return -1;
        }
    }

    std::stack<double> values;
    std::stack<char> operations;

    values.push(0);

    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i] == "(") {
            operations.push(tokens[i][0]);
        } else if (isdigit(tokens[i][0])) {
            double num = std::stod(tokens[i]);
            values.push(num);
        } else if (tokens[i] == ")") {
            while (!operations.empty() && operations.top() != '(') {
                double val2 = values.top();
                values.pop();

                double val1 = values.top();
                values.pop();

                char op = operations.top();
                operations.pop();

                values.push(applyOp(val1, val2, op));
            }
            operations.pop();
        } else {
            while (!operations.empty() && precedence(operations.top()) >= precedence(tokens[i][0])) {
                double val2 = values.top();
                values.pop();

                double val1 = values.top();
                values.pop();

                char op = operations.top();
                operations.pop();

                values.push(applyOp(val1, val2, op));
            }
            operations.push(tokens[i][0]);
        }
    }

    while (!operations.empty()) {
        double val2 = values.top();
        values.pop();

        double val1 = values.top();
        values.pop();

        char op = operations.top();
        operations.pop();

        values.push(applyOp(val1, val2, op));        
    }

    double ans = values.top();
    // getNextToken();
    return ans;

}

void HandleIdentifier() {
    if (CurTok != tok_identifier) {
        LogError("Invalid Identifier");
        getNextToken();
        return;
    }
    std::string firstIdentifier = IdentifierStr;
    getNextToken();

    if (CurTok == tok_operator && Operator == "=") {
        // Evaluate RHS
        getNextToken();
        double res = EvaluateRHS();
        if (invalidSyntax) {
            invalidSyntax = false;
            getNextToken();
            return;
        } else {
            data[firstIdentifier] = res;
            getNextToken();
        }
    } else {
        if (data.find(firstIdentifier) == data.end()) {
            LogError("Identifier not found");
            return;
        } else {
            double val = data[firstIdentifier];
            fprintf(stderr, "%f\n", val);
            getNextToken();
        }
    }
}

void HandlePrint() {
    getNextToken();
    if (CurTok == tok_number) {
        double res = EvaluateRHS();
        fprintf(stderr, "%f\n", res);
        getNextToken();
        return;
    } else if (CurTok == tok_identifier) {
        std::string firstIdentifier = IdentifierStr;
        if (data.find(firstIdentifier) == data.end()) {
            LogError("Identifier not found");
            getNextToken();
            return;
        } else {
            double val = data[firstIdentifier];
            fprintf(stderr, "%f\n", val);
            getNextToken();
        }
    } else {
        LogError("Invalid Syntax");
        getNextToken();
    }
}

void HandleNumber() {
    double res = EvaluateRHS();
    fprintf(stderr, "%f\n", res);
}

static void MainLoop() {
    int lineNumber = 1;
    while(true) {
        lineNumber++;
        fprintf(stderr, ">>>: ");
        switch (CurTok) {
        case tok_eof:
            return;
        case tok_identifier:
            HandleIdentifier();
            break;
        case tok_number:
            HandleNumber();
            break;
        case tok_print:
            HandlePrint();
            break;
        case tok_endline:
            getNextToken();
            break;
        case tok_operator:
            HandleNumber();
            break;
        default:
            break;
        }
    }
}

int main() {
    fprintf(stderr, ">>>: ");
    getNextToken();
    MainLoop();
    return 0;
}