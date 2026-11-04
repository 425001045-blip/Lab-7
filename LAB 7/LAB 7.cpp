#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <cmath>
#include <vector>

// ==========================================
// PART 1: Custom Template Stack<T>
// ==========================================
template <typename T>
class Stack {
private:
    T* data;
    size_t cap;
    size_t sz;

    void resize(size_t new_cap) {
        T* new_data = new T[new_cap];
        for (size_t i = 0; i < sz; ++i) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        cap = new_cap;
    }

public:
    // Constructor: begins with capacity 2
    Stack() : cap(2), sz(0) {
        data = new T[cap];
    }

    // Destructor: releases storage[cite: 3]
    ~Stack() {
        delete[] data;
    }

    // Prevent unsafe shallow copying[cite: 3]
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    // push(value): adds value to TOP, size grows by 1[cite: 1]
    void push(const T& value) {
        if (sz == cap) {
            resize(cap * 2); // Double capacity when full[cite: 3]
        }
        data[sz++] = value;
    }

    // pop(value): removes and returns TOP, returns false if empty[cite: 1, 3]
    bool pop(T& value) {
        if (isEmpty()) {
            return false;
        }
        value = data[--sz];
        return true;
    }

    // peek(value): reads TOP without removing, returns false if empty[cite: 1, 3]
    bool peek(T& value) const {
        if (isEmpty()) {
            return false;
        }
        value = data[sz - 1];
        return true;
    }

    bool isEmpty() const {
        return sz == 0;
    }

    size_t size() const {
        return sz;
    }

    // Helper to display stack contents from bottom to top for debugging/demo
    void displayBottomToTop() const {
        std::cout << "Stack (bottom to top): [ ";
        for (size_t i = 0; i < sz; ++i) {
            std::cout << data[i] << " ";
        }
        std::cout << "]\n";
    }
};

// ==========================================
// PART 2: Bracket Validation
// ==========================================
bool checkBrackets(const std::string& expr, std::string& errorMessage) {
    Stack<char> s;
    for (size_t i = 0; i < expr.length(); ++i) {
        char ch = expr[i];
        if (ch == '(' || ch == '[' || ch == '{') {
            s.push(ch); // Push every opening bracket[cite: 4, 5]
        }
        else if (ch == ')' || ch == ']' || ch == '}') {
            if (s.isEmpty()) {
                errorMessage = "Unmatched closing bracket '" + std::string(1, ch) + "'";
                return false;
            }
            char topVal;
            s.pop(topVal);
            // Verify matching bracket type[cite: 4]
            if ((ch == ')' && topVal != '(') ||
                (ch == ']' && topVal != '[') ||
                (ch == '}' && topVal != '{')) {
                errorMessage = "Mismatched pair: expected closing for '" + std::string(1, topVal) + "' but found '" + std::string(1, ch) + "'";
                return false;
            }
        }
    }
    if (!s.isEmpty()) {
        errorMessage = "Unclosed opening bracket(s) remaining";
        return false;
    }
    errorMessage = "Balanced";
    return true;
}

// ==========================================
// PART 3: Tokenization & Infix to Postfix
// ==========================================
int getPrecedence(char op) {
    if (op == '*' || op == '/') return 2; // Precedence 2[cite: 6]
    if (op == '+' || op == '-') return 1; // Precedence 1[cite: 6]
    return 0;
}

bool tokenizeAndConvertToPostfix(const std::string& infix, std::string& postfixOutput, std::string& errorMsg) {
    // First run bracket check
    if (!checkBrackets(infix, errorMsg)) {
        return false;
    }

    Stack<char> opStack;
    std::string postfix = "";

    enum State { EXPECT_OPERAND, EXPECT_OPERATOR };
    State state = EXPECT_OPERAND;

    size_t i = 0;
    while (i < infix.length()) {
        char ch = infix[i];

        if (isspace(ch)) {
            i++;
            continue;
        }

        // Handle multi-digit numbers
        if (isdigit(ch)) {
            if (state == EXPECT_OPERATOR) {
                errorMsg = "Syntax error: number after number or closing bracket without operator";
                return false;
            }
            std::string numStr = "";
            while (i < infix.length() && isdigit(infix[i])) {
                numStr += infix[i];
                i++;
            }
            postfix += numStr + " ";
            state = EXPECT_OPERATOR;
            continue;
        }

        // Handle brackets and operators
        if (ch == '(' || ch == '[' || ch == '{') {
            if (state == EXPECT_OPERATOR) {
                errorMsg = "Syntax error: implicit multiplication or '(' after number/bracket not allowed";
                return false;
            }
            opStack.push('('); // Treat all grouping symbols as '(' for conversion[cite: 2, 6]
            state = EXPECT_OPERAND;
            i++;
        }
        else if (ch == ')' || ch == ']' || ch == '}') {
            if (state == EXPECT_OPERAND) {
                errorMsg = "Syntax error: empty group or unexpected closing bracket";
                return false;
            }
            char topOp;
            bool matchedOpen = false;
            while (opStack.peek(topOp)) {
                if (topOp == '(') {
                    opStack.pop(topOp);
                    matchedOpen = true;
                    break;
                }
                opStack.pop(topOp);
                postfix += std::string(1, topOp) + " ";
            }
            if (!matchedOpen) {
                errorMsg = "Mismatched brackets during conversion";
                return false;
            }
            state = EXPECT_OPERATOR;
            i++;
        }
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            if (state == EXPECT_OPERAND) {
                errorMsg = "Syntax error: operator where operand was expected (e.g., unary sign or operator sequence)";
                return false;
            }
            // Pop operators of greater or equal precedence (left-associative rule >=)[cite: 2, 6]
            char topOp;
            while (opStack.peek(topOp) && topOp != '(' && getPrecedence(topOp) >= getPrecedence(ch)) {
                opStack.pop(topOp);
                postfix += std::string(1, topOp) + " ";
            }
            opStack.push(ch);
            state = EXPECT_OPERAND;
            i++;
        }
        else {
            errorMsg = "Unsupported character: '" + std::string(1, ch) + "'";
            return false;
        }
    }

    if (state == EXPECT_OPERAND) {
        errorMsg = "Trailing operator or incomplete expression";
        return false;
    }

    // Pop remaining operators[cite: 6]
    char topOp;
    while (opStack.peek(topOp)) {
        if (topOp == '(') {
            errorMsg = "Leftover opening bracket";
            return false;
        }
        opStack.pop(topOp);
        postfix += std::string(1, topOp) + " ";
    }

    postfixOutput = postfix;
    return true;
}

// ==========================================
// PART 4: Evaluate Postfix Expression
// ==========================================
bool evaluatePostfix(const std::string& postfix, double& result, std::string& errorMsg) {
    Stack<double> valStack;
    std::stringstream ss(postfix);
    std::string token;

    while (ss >> token) {
        // Check if token is a number
        if (isdigit(token[0])) {
            valStack.push(std::stod(token));
        }
        else if (token == "+" || token == "-" || token == "*" || token == "/") {
            double right, left;
            if (!valStack.pop(right) || !valStack.pop(left)) {
                errorMsg = "Evaluation error: missing operands for operator " + token;
                return false;
            }
            double res = 0;
            if (token == "+") res = left + right;
            else if (token == "-") res = left - right;
            else if (token == "*") res = left * right;
            else if (token == "/") {
                if (right == 0.0) {
                    errorMsg = "Division by zero error";
                    return false;
                }
                res = left / right; // Uses double values[cite: 2]
            }
            valStack.push(res);
        }
        else {
            errorMsg = "Invalid token in postfix evaluation: " + token;
            return false;
        }
    }

    if (valStack.size() != 1) {
        errorMsg = "Evaluation error: malformed expression stack state";
        return false;
    }

    valStack.pop(result);
    return true;
}

// ==========================================
// PART 5: Integrated Menu & Stack Demo
// ==========================================
void runStackDemo() {
    std::cout << "\n--- Stack Demonstration (Part 1, Step 6 Test Walk-through) ---\n";
    Stack<int> demoStack;
    int val;

    std::cout << "1. pop() on empty stack -> " << (demoStack.pop(val) ? "true" : "false") << "; size = " << demoStack.size() << "\n";

    demoStack.push(10);
    demoStack.push(20);
    std::cout << "2. push(10), push(20), peek() -> ";
    if (demoStack.peek(val)) std::cout << val;
    std::cout << "; size = " << demoStack.size() << "\n";

    demoStack.push(30);
    std::cout << "3. push(30) -> array grows/node added; size = " << demoStack.size() << "\n";
    demoStack.displayBottomToTop();

    std::cout << "4. Pop three times: ";
    while (demoStack.pop(val)) {
        std::cout << val << " ";
    }
    std::cout << "\n5. peek() on empty stack -> " << (demoStack.peek(val) ? "true" : "false") << "; no crash, size = " << demoStack.size() << "\n";
}

int main() {
    int choice = -1;
    std::string inputLine;

    do {
        std::cout << "\n========================================\n";
        std::cout << " NATIONAL TEACHERS COLLEGE - CC105 Lab  \n";
        std::cout << "========================================\n";
        std::cout << "1. Check brackets\n";
        std::cout << "2. Convert and evaluate expression\n";
        std::cout << "3. Run stack demonstration\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter choice: ";

        if (!std::getline(std::cin, inputLine)) break;
        try {
            choice = std::stoi(inputLine);
        } catch (...) {
            std::cout << "Invalid input choice. Please enter a valid number[cite: 10].\n";
            continue;
        }

        switch (choice) {
            case 1: {
                std::cout << "Enter expression to check brackets: ";
                std::getline(std::cin, inputLine);
                std::string err;
                bool status = checkBrackets(inputLine, err);
                std::cout << "Result: " << (status ? "Balanced[cite: 4]" : ("Error: " + err)) << "\n";
                break;
            }
            case 2: {
                std::cout << "Enter arithmetic expression: ";
                std::getline(std::cin, inputLine);
                std::string postfix, err;
                if (tokenizeAndConvertToPostfix(inputLine, postfix, err)) {
                    std::cout << "Bracket Status: Balanced[cite: 2, 4]\n";
                    std::cout << "Postfix Expression: " << postfix << "\n";
                    double res;
                    if (evaluatePostfix(postfix, res, err)) {
                        std::cout << "Evaluation Result: " << res << "\n";
                    } else {
                        std::cout << "Evaluation Error: " << err << "\n";
                    }
                } else {
                    std::cout << "Validation/Conversion Error: " << err << "\n";
                }
                break;
            }
            case 3:
                runStackDemo();
                break;
            case 0:
                std::cout << "Exiting application. Destructors running[cite: 10].\n";
                break;
            default:
                std::cout << "Invalid choice. Please choose between 0 and 3[cite: 10].\n";
        }
    } while (choice != 0);

    return 0;
}
