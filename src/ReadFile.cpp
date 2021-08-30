#include <iostream>
#include <exception>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <cstdlib>
#include <memory>

#include "Token.hpp"
#include "ReadFile.hpp"
#include "Parser.hpp"
#include "Scanner.hpp"
#include "RuntimeError.hpp"
#include "Interpreter.hpp"
#include "Stmt.hpp"

using std::cout;
using std::cin;
using std::exception;
using std::cerr;
using std::string;
using std::ifstream;
using std::istreambuf_iterator;


bool ReadFile::hadError=false;
bool ReadFile::hadRuntimeError=false;

Interpreter interpreter{};

int ReadFile::runScript(int argc, const char* argv[]) {
      try
    {   
        if(argc>2) {
            cout <<("Usage: jlox [script]")<<'\n';
        } 
        else if (argc == 2) {
            runFile(argv[1]);
        } else {
            runPromt();
        }
    }
    catch(const exception& e)
    {
        cerr << e.what() << '\n';
        return 1;
    }
    return 0;
};
void ReadFile::run( string input){
        Scanner scanner {input};
        std::vector<Token> tokens = scanner.scanTokens();
        
        Parser parser(tokens);
        std::vector<shared_ptr<Stmt>> statements = parser.parse();
        if(hadError) {
            return;
        }
        if(hadRuntimeError) {
            return;
        }
        interpreter.interpret(statements);
        //cout<<tokens<<'\n';
        //for (int i=0; i <tokens.size(); ++i)
        //std::cout << tokens.at(2) << ' ';
        //printVec(tokens);
        //std::copy(tokens.begin(),
        //tokens.end(),
        //std::ostream_iterator<Token>(cout, " "));
        //cout<<"hej"<<"\n";
    }

void ReadFile::runFile( string path){
        //ifstream in (path);
        ifstream ifs(path);
        //in.exceptions(ifstream::failbit | ifstream::badbit);
        //read input into std::string
        string file_data((std::istreambuf_iterator<char>(ifs)),
                 std::istreambuf_iterator<char>());
        //cout<< file_data << "\n";
        //cout<<"END OF FILE DATA"<<"\n";
        run(file_data);
        if(hadError) {
            return;
        }
        if(hadRuntimeError) {
            return;
        }    }

void ReadFile::runPromt() {
    for (;;) {
        std::cout << "> ";
        std::string line;
        if (!std::getline(std::cin, line)) break;
        run(line);
        hadError = false;
  }
}

void ReadFile::report(int line, string description ,string message) {
    cout << message<< description<< "at line "+line<<"\n";
    hadError=true;
}

void ReadFile::error(Token token,string msg) {
    if (token.type == EOF_TOKEN) {
        report(token.line, " at end", msg);
    } else {
        report(token.line, " at '" + token.lexeme + "'", msg);
    }
  }

void ReadFile::runtimeError( RuntimeError &error) {
    cout<< "line" + error.token.line << "\n";
    hadRuntimeError = true;
}


