#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <cctype>
#include <cmath>

enum class TokenType {
    IDENTIFIER, NUMBER, STRING, ASSIGN,
    PLUS, MINUS, EQUALS,
    PHRASE, NL, COMPL, SEMICOLON,
    IOV, NU, DECI, SINCH, LOOP, IF, ELSE, END,
    TOKEN, LPAREN, RPAREN, DOT, COMMA
};

struct Token {
    TokenType type;
    std::string value;
};

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < input.length()) {
        if (std::isspace(input[i])) {
            ++i;
        } else if (std::isalpha(input[i])) {
            std::string word;
            while (i < input.length() && (std::isalnum(input[i]) || input[i] == '_')) {
                word += input[i++];
            }
            if (word == "phrase") tokens.push_back({TokenType::PHRASE, word});
            else if (word == "complem") tokens.push_back({TokenType::COMPL, word});
            else if (word == "iov") tokens.push_back({TokenType::IOV, word});
            else if (word == "nuv" || word == "nu") tokens.push_back({TokenType::NU, word});
            else if (word == "deci") tokens.push_back({TokenType::DECI, word});
            else if (word == "sinch") tokens.push_back({TokenType::SINCH, word});
            else if (word == "loop") tokens.push_back({TokenType::LOOP, word});
            else if (word == "if") tokens.push_back({TokenType::IF, word});
            else if (word == "else") tokens.push_back({TokenType::ELSE, word});
            else if (word == "token") tokens.push_back({TokenType::TOKEN, word});
            else if (word == "nl") tokens.push_back({TokenType::NL, word});
            else tokens.push_back({TokenType::IDENTIFIER, word});
        } else if (std::isdigit(input[i])) {
            std::string num;
            while (i < input.length() && std::isdigit(input[i])) {
                num += input[i++];
            }
            tokens.push_back({TokenType::NUMBER, num});
        } else if (input[i] == '"') {
            ++i;
            std::string str;
            while (i < input.length() && input[i] != '"') {
                str += input[i++];
            }
            ++i;
            tokens.push_back({TokenType::STRING, str});
        } else if (input[i] == '=' && i + 1 < input.length() && input[i + 1] == '=') {
            tokens.push_back({TokenType::EQUALS, "=="});
            i += 2;
        } else if (input[i] == '=') {
            tokens.push_back({TokenType::ASSIGN, "="});
            ++i;
        } else if (input[i] == '+') {
            tokens.push_back({TokenType::PLUS, "+"});
            ++i;
        } else if (input[i] == '-') {
            tokens.push_back({TokenType::MINUS, "-"});
            ++i;
        } else if (input[i] == ';') {
            tokens.push_back({TokenType::SEMICOLON, ";"});
            ++i;
        } else if (input[i] == '(') {
            tokens.push_back({TokenType::LPAREN, "("});
            ++i;
        } else if (input[i] == ')') {
            tokens.push_back({TokenType::RPAREN, ")"});
            ++i;
        } else if (input[i] == '.') {
            tokens.push_back({TokenType::DOT, "."});
            ++i;
        } else if (input[i] == ',') {
            tokens.push_back({TokenType::COMMA, ","});
            ++i;
        } else {
            ++i;
        }
    }

    tokens.push_back({TokenType::END, ""});
    return tokens;
}

class Interpreter {
    std::vector<Token> tokens;
    size_t current = 0;
    std::unordered_map<std::string, std::string> stringVars;
    std::unordered_map<std::string, int> intVars;
    std::unordered_map<std::string, double> deciVars; // New: support for decimals

    Token peek() { return tokens[current]; }
    Token advance() { return tokens[current++]; }
    bool match(TokenType type) {
        if (peek().type == type) {
            advance();
            return true;
        }
        return false;
    }

    int evaluateExpression() {
        Token value = advance();
        int result = 0;
        if (value.type == TokenType::NUMBER) result = std::stoi(value.value);
        else if (value.type == TokenType::IDENTIFIER && intVars.count(value.value)) result = intVars[value.value];

        while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
            Token op = advance();
            Token next = advance();
            int rhs = (next.type == TokenType::NUMBER) ? std::stoi(next.value) : intVars[next.value];
            result = (op.type == TokenType::PLUS) ? result + rhs : result - rhs;
        }
        return result;
    }

    void statement() {
        if (match(TokenType::TOKEN)) {
            if (match(TokenType::LPAREN)) {
                while (!match(TokenType::RPAREN)) {
                    Token type = advance();
                    match(TokenType::DOT);
                    std::string name = advance().value;
                    if (type.type == TokenType::IOV) stringVars[name] = "";
                    else if (type.type == TokenType::NU) intVars[name] = 0;
                    else if (type.type == TokenType::SINCH) stringVars[name] = "";
                    if (!match(TokenType::COMMA)) break;
                }
            }
        } else if (match(TokenType::PHRASE)) {
            while (peek().type != TokenType::END && peek().type != TokenType::SEMICOLON) {
                Token value = advance();
                if (value.type == TokenType::STRING) {
                    std::cout << value.value;
                } else if (value.type == TokenType::IDENTIFIER) {
                    if (stringVars.count(value.value)) std::cout << stringVars[value.value];
                    else if (intVars.count(value.value)) std::cout << intVars[value.value];
                    else if (deciVars.count(value.value)) std::cout << deciVars[value.value]; // Added decimal support
                }
            }
            std::cout << std::endl;
            if (peek().type == TokenType::SEMICOLON) advance();
        } else if (match(TokenType::NL)) {
            std::cout << std::endl;
        } else if (match(TokenType::COMPL)) {
            Token var = advance();
            std::string input;
            std::getline(std::cin, input);
            if (intVars.count(var.value)) intVars[var.value] = std::stoi(input);
            else if (deciVars.count(var.value)) deciVars[var.value] = std::stod(input); // Handle decimals
            else stringVars[var.value] = input;
        } else if (match(TokenType::IOV)) {
            // Handle 'iov' assignment
            std::string varName = advance().value;  // Variable name
            match(TokenType::ASSIGN);  // Expect '='
            Token next = advance();

            if (next.type == TokenType::STRING) {
                // Assign string value to 'iov'
                stringVars[varName] = next.value;
            } else if (next.type == TokenType::NUMBER) {
                // If it's a number, assign to intVars
                intVars[varName] = std::stoi(next.value);
            } else if (next.type == TokenType::DECI) {
                // Handle decimal assignment for 'deci'
                deciVars[varName] = std::stod(next.value);
            }
        } else if (peek().type == TokenType::IDENTIFIER) {
            std::string var = advance().value;
            if (match(TokenType::EQUALS)) {
                Token next = peek();
                if (next.type == TokenType::STRING) {
                    stringVars[var] = advance().value;
                } else if (next.type == TokenType::NUMBER) {
                    intVars[var] = std::stoi(advance().value);
                } else if (next.type == TokenType::DECI) {
                    deciVars[var] = std::stod(advance().value);
                }
            }
        } else {
            advance();
        }
    }

public:
    Interpreter(const std::vector<Token>& t) : tokens(t) {}
    void run() {
        while (peek().type != TokenType::END) {
            statement();
        }
    }
};

int main() {
    std::stringstream input;
    std::string line;
    std::cout << "Type your code (type 'exit' to run):\n";
    while (true) {
        std::getline(std::cin, line);
        if (line == "exit") break;
        input << line << "\n";
    }
    auto tokens = tokenize(input.str());
    Interpreter interpreter(tokens);
    interpreter.run();
    return 0;
}
