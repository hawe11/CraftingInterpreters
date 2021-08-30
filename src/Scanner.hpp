#ifndef Scanner_HPP_
#define Scanner_HPP_


#include "Token.hpp"

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

class Scanner {
    private: 
        string const source;
        vector <Token> scannedTokens;
        static map<string, TokenType> keywords;
        void scanToken();
        int start=0;
        int line=1;
        bool isAtEnd();
        char advance();
        void addToken(TokenType type);
        void addToken(TokenType type,Object literal);
        bool match(char expected);
        char peek();
        void getString();
        bool isDigit(char c);
        void getNumber();
        char peekNext();
        bool isAlpha(char c);
        void identifier();
        bool isAlphaNumeric(char c);
    public:
        int current=0;

        Scanner(string source);
        vector<Token> scanTokens();
};

#endif