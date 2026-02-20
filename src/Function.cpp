#include "Function.hpp"
#include <iostream>



    int Function::getPrecedence(char op) { //
        switch(op) {
            case '+': return 2;
            case '-': return 2;
            case '*': return 3;
            case '/': return 3;
            case '^': return 4;
            default: return 0;
        }
    }

    bool Function::getLeftAssociativity(char op) {//
        switch(op){
            case '+': return true;
            case '-': return true;
            case '*': return true;
            case '/': return true;
            case '^': return false;
            default: return false;
        }
    }

    bool Function::isOperator(char c) { //
        return c == '+' || c == '-' || c == '*' || 
            c == '/' || c == '%' || c == '^';
    }

    void Function::ShuntingYard(FunctionData& function) //
    {


        std::stack<std::string> output;
        std::stack<std::string> operators;
        std::stack<std::string> tempOut;

        std::string temp;
        float tempResult = 0.0f;
        std::string expr = function.expression;



        for (int i=0; i <= expr.size(); i++){
            //init var used to detect function ( like sin )
            int j=0;
            bool function = false;
            //using a temp var for common processing 
            temp = expr[i];


            if (std::isdigit(temp[0]) || temp[0] == 'x' || temp[0] == 'y'){
                output.push(temp);
                //std::cout << "digit\n";
            } else if (std::isalpha(temp[0]) != 0){
                while (!function){
                    j++;
                    if (std::isalpha(expr[j+i]) != 0) continue;
                    else function = true; operators.push(expr.substr(i, j)); i += j;
                }
            } else if (isOperator(temp[0])) {
                if (operators.empty()) operators.push(temp);
                else { 
                    while(!operators.empty() && operators.top()[0] != '(' && (getPrecedence(operators.top()[0]) > getPrecedence(temp[0]) || (getPrecedence(temp[0]) == getPrecedence(operators.top()[0]) && getLeftAssociativity(temp[0])))){
                        output.push(operators.top()) ; operators.pop(); 
                    }
                operators.push(temp);
                }
            } else if ((temp[0] == '(')){
                operators.push(temp);
            } else if (temp[0] == ')'){
                    while( !operators.empty() && operators.top()[0] != '('){
                        output.push(operators.top()) ; operators.pop();
                    }
                    if (!operators.empty()) operators.pop(); // Pop the left parenthesis
            }

        }

        while (!operators.empty()) {
            output.push(operators.top());
            operators.pop();
        }

        std::stack<std::string> tempV(output);  // make a copy
        std::vector<std::string> v;

        while (!tempV.empty()) {
            v.push_back(tempV.top());
            tempV.pop();
        }

        std::reverse(v.begin(), v.end());
        function.ShuntingYardExpr = v;

        std::cout << "RPN Expresion :";
        for (std::string c : function.ShuntingYardExpr) std::cout << c << " ";
        std::cout << '\n';
    }

    double Function::applyFunctionRPN(const std::string& func, std::stack<double>& s) {
        if (func == "sqrt") {
            double a = s.top(); s.pop();
            return std::sqrt(a);
        }
        else if (func == "sin") {
            double a = s.top(); s.pop();
            return std::sin(a);
        }
        else if (func == "cos") {
            double a = s.top(); s.pop();
            return std::cos(a);
        }
        else if (func == "exp"){
            double a = s.top(); s.pop();
            return std::exp(a);
        }

        throw std::runtime_error("Unknown function");
    }

    float Function::RPNCalculator(FunctionData& function, float x, float y) //
    {

        std::stack<double> s;
        std::vector<std::string> tokens = function.ShuntingYardExpr;


        for (const std::string& token : tokens) {
            if (std::isdigit(token[0])) {  // simple check for number
                s.push(std::stod(token));
            }
            else if (token == "x") s.push(x);
            else if (token == "y") s.push(y);
            else if (isOperator(token[0]))  {  // operator
                if (s.size() < 2) {
                    //std::cerr << "Invalid expression: not enough operands\n";
                    continue;
                }
                double b = s.top(); s.pop();
                double a = s.top(); s.pop();
                if (token == "+"){ s.push(a+b);}
                else if (token == "-"){ s.push(a-b);}
                else if (token == "*"){ s.push(a*b);}
                else if (token == "/"){ s.push(a/b);}
                else if (token == "^"){ s.push(pow(a,b));}
            }
            else {
                s.push(applyFunctionRPN(token, s));
            }
        }

        float result = s.top();

        if (result > function.yMax) function.yMax = result;
        if (result < function.yMin) function.yMin = result;

        return result;
    }

