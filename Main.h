/* 
 * File:   Main.h
 * Author: hugh
 *
 * Created on July 26, 2016, 7:07 PM
 */

#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <string> 

#ifdef _WIN32

    #include <windows.h>
    #include <Shellapi.h>
    #include <direct.h>

#endif

#include <cstdlib>
#include <fstream>
#include <streambuf>
#include <assert.h>
#include <exception>
#include <deque>
#include <algorithm>
#include <iterator>
#include "json/src/json.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/join.hpp"

using namespace std;
using json = nlohmann::json;

#ifndef STRRSPN
/*
  A non-standard helper function:
  Find c such that s=".*c[d]*", where [d]=delimiters and c in [d].
  If c is not found, it returns the argument s.
*/
const char* strrspn( const char* s, const char* delimiters );
#endif

char* trim_left_advance( char* s, const char* delimiters );

class Main {

    public:
        Main();
        void usage(char* prog); 
        void mainDriver(char* inputFilePath, char* delimiter, char* nodeKey);
        static deque<string> split(string str, char delimiter);
        static void display_deque(const deque<string> &v);
        static const char internalDelimiterChar();
        static const string internalDelimiterString();
        static const string join(deque<string> strs, string delimiter);
        static void writeStringToFile(string s, string filePath);
        static const string getResultsDirectory();
        static void deleteResultsDirectory();
        static void createResultsDirectory();
};

#endif /* MAIN_H */