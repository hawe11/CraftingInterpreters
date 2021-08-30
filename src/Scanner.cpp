
#include "Token.hpp"
#include "Scanner.hpp"
#include "ReadFile.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <map>

using std::string;
using std::vector;
using std::stod;
using std::map;


map<string, TokenType> Scanner::keywords = {
    {"and", AND},
    {"class",  CLASS},
    {"else",   ELSE},
    {"false",  FALSE},
    {"for",    FOR},
    {"fun",    FUN},
    {"if",     IF},
    {"nil",    NIL},
    {"or",     OR},
    {"print",  PRINT},
    {"return", RETURN},
    {"super",  SUPER},
    {"this",   THIS},
    {"true",   TRUE},
    {"var",    VAR},
    {"while",  WHILE},
};


Scanner::Scanner(string source) : source(source) {}

vector<Token> Scanner::scanTokens() {
    while (!isAtEnd())
    {
        start=current;
        scanToken();
    }
    Token token(EOF_TOKEN,"",Object::make_str_object(""), line);
    scannedTokens.push_back(token);
    return scannedTokens;
    
}

bool Scanner::isAtEnd() {
    return current >= source.size();
}

char Scanner::advance() {
    return source.at(current++);
}

void Scanner::addToken(TokenType type) {
    Scanner::addToken(type,Object::make_str_object(""));

}

void Scanner::addToken(TokenType type, Object literal) {
    string text = source.substr(start,current-start);
    Token token(type,text,literal,line);
    scannedTokens.push_back(token);
}

void Scanner::scanToken() {
    char c = advance();
    switch(c) {
        case '(': addToken(LEFT_PAREN); break;
        case ')': addToken(RIGHT_PAREN); break;
        case '{': addToken(LEFT_BRACE); break;
        case '}': addToken(RIGHT_BRACE); break;
        case ',': addToken(COMMA); break;
        case '.': addToken(DOT); break;
        case '-': addToken(MINUS); break;
        case '+': addToken(PLUS); break;
        case ';': addToken(SEMICOLON); break;
        case '*': addToken(STAR); break;
        case '!': addToken(match('=') ? BANG_EQUAL : BANG); break;
        case '=': addToken(match('=') ? EQUAL_EQUAL : EQUAL); break;
        case '<': addToken(match('=') ? LESS_EQUAL : LESS); break;
        case '>': addToken(match('=') ? GREATER_EQUAL : GREATER); break;
        case '/':
            if(match('/')) {

                while(peek() != '\n' && isAtEnd()) {
                    if (peek() == '\n' && !isAtEnd()){
                        advance();
                    }
                }
            } else {
                addToken(SLASH);
            }
            break;

      case ' ':
      case '\r':
      case '\t':
        // Ignore whitespace.
        break;

      case '\n':
        line++;
        break;
        case '"':
            getString();
            break;
        default :
            if (isDigit(c)) {
                getNumber();
            } else if (isAlpha(c)){
                identifier();
            }
            else {
                ReadFile::report(line,"at line","Unexpeced character.");
            } 
             break;
    }
    }
    
bool Scanner::match(char expected) {
    if(isAtEnd()) {
        return false;
    }
    if(source.at(current) != expected) {
        return false;
    }
    current++;
    return true;
}


char Scanner::peek(){
    if(isAtEnd()) {
        return '\0';
    }
    return source.at(current);
}

void Scanner::getString() {
    while (peek() !='"' && !isAtEnd()) {
        if (peek() == '/n') {
            line++;
        }
        advance();
    }
    if (isAtEnd()) {
        ReadFile::report(line,"at line","Unterminated string");
    }

    advance();

    string value = source.substr(start,current-start);
    addToken(STRING,Object::make_str_object(value));
}

bool Scanner::isDigit(char c) {
    return c >= '0' && c <= '9';
}

void Scanner::getNumber() {
    while(isDigit(peek())) {
        advance();
    }
    // look for fractional part '.'
    if(peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isDigit(peek())) {
            advance();
        }

    }
        double num=stod(source.substr(start,current));
        addToken(NUMBER,Object::make_double_object(num));
}

char Scanner::peekNext() {
    if(current +1 >=source.length()) {
        return '/0';
    } 
    return source.at(current+1);
}

bool Scanner::isAlpha(char c) {
   return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

void Scanner::identifier() {
    TokenType type;

    while (isAlphaNumeric(peek())){
        advance();
    }
    string text = source.substr(start, current-start);

    auto found_element = keywords.find(text);
    if (found_element == keywords.end()) {
        type = IDENTIFIER;
    } else {
        type = found_element->second;

    }
    addToken(type);

}

bool Scanner::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}