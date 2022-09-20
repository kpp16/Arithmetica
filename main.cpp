/**
 * @file main.cpp
 * @author Kairav Parekh
 * @brief The main C++ file that contains the lexer, parser and interpreter
 *        Check the README file to learn the rules of this interpreter
 * @version 0.1
 * @date 2022-09-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <stack>
#include <string>

/**
 * @brief The each number repersents 
 * what kind of token the lexer scanned
 */
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

static std::string IdentifierStr = "";
static double NumVal;
static std::string Operator;

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

/**
 * @brief Get the Next Token object
 * 
 * @return int 
 */
static int getNextToken() {
    return CurTok = gettok();
}

/**
 * @brief A helper method to print out an error
 * 
 * @param Str 
 * @return int 
 */
int LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return -1;
}

/**
 * @brief Returns precedence of an arithmetical operator
 * 
 * @param op 
 * @return int 
 */
int precedence(char op) {
    if(op == '+'||op == '-')
        return 1;
    if(op == '*'||op == '/')
        return 2;
    return 0;
}

/**
 * @brief Method to carry out an operaton on the operands
 *        depending on the operator
 * 
 * @param a The first operand
 * @param b The second operand
 * @param op The operator
 * @return double The result after performing the operation
 */
double applyOp(double a, double b, char op) {
    switch(op){
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        default: return -1;
    }
}

bool invalidSyntax = false;

/**
 * @brief Evalute an arithemtic  expression
 * 
 * @return double 
 * The final result after evaluating an arithmetic expression
 */
double EvaluateRHS() {
    if (CurTok == tok_endline) {
        LogError("Invalid Syntax");
        getNextToken();
        invalidSyntax = true;
        return -1;
    }
    std::vector<std::string> tokens;
    
    // Add each token to a vector of strings
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

    // Evalute the expression using 2 stacks

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
    return ans;

}

/**
 * @brief Method to handle an identifier
 * If there is an '=' followed, it would indicate that there is an expression followed by
 * Else, just print out the value stored in a variable with the name as the identifier
 * 
 */
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

/**
 * @brief Handle the print command
 * 
 * Eat up the "print" and then evalute the expression that is followed by
 * 
 */
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

/**
 * @brief Method to handle numbers input in the console
 * 
 */
void HandleNumber() {
    double res = EvaluateRHS();
    fprintf(stderr, "%f\n", res);
}

/**
 * @brief The mainloop
 * 
 */
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