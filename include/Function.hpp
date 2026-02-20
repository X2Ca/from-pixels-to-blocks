#pragma once

#include <stack>
#include <string>
#include <vector>

struct FunctionData
{
    float yMin;
    float yMax;
    std::string expression;
    std::vector<std::string> ShuntingYardExpr;
};

class Function {
public:
    Function() = default;
    void ShuntingYard(FunctionData& function);
    float RPNCalculator(FunctionData& function, float x, float y);

private:
    int getPrecedence(char op);
    bool getLeftAssociativity(char op);
    bool isOperator(char c);
    double applyFunctionRPN(const std::string& func, std::stack<double>& s);
    
};