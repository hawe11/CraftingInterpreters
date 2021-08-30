#ifndef ReadFile_HPP_
#define ReadFile_HPP_


#include "RuntimeError.hpp"

#include <string>
#include <vector>
using std::string;

class ReadFile {
    public:
        
        static int runScript(int argc, const char* argv[]);
        static void report(int line, string description,string message);

//    private:
        static void error(Token token,string msg);
        static bool hadError;
        static bool hadRuntimeError;
        static void runFile(string path);
        static void runPromt();
        static void run(string input);
        static void runtimeError(RuntimeError &error);
};
#endif