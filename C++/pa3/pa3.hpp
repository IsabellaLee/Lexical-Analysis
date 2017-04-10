//
//  pa3.h
//  ProjectAssign#3
//
//  Created by Lin Li on 3/29/17.
//  Copyright © 2017 Isabella Lee. All rights reserved.
//

#include <map>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iterator>
#include <deque>

#ifndef pa3_h
#define pa3_h

using namespace std;

class Compiler {
public:
    int loops = 0; //Number of nested loops in codes
    vector<string> codes; //Content of .txt file, which is codes
    
    /*
     Use unordered_map to present the keywords, identifiers, constants
     operators, and delimiters so that each element is unique in each
     unordered_map
     */
    
    unordered_map<string, int> keywords;
    unordered_map<string, int> identifiers;
    unordered_map<string, int> constants;
    unordered_map<string, int> operators;
    unordered_map<char, int> delimiters;
   
    //Constructor of Compiler
    Compiler(vector<string> codes);
    
    //Go through the codes
    void read();
    //Belows are print methods for different data strcutures
    
    //Print out the syntax errors
    void printSyntaxErrors();
    
    //Print out the object of unordered_map<string, int> data type
    void printMap(unordered_map<string, int> map);
    
    //Print out the object of unordered_map<char, int> data type
    void printMap (unordered_map<char, int> map);

private:
    vector<string> syntaxErrors; //Represents a list of syntax errors
    vector<string> words; //a list of keywords
    
    //Find keywords
    void detectKeyWords(string lineOfCode);
    
    //Find identifiers
    void detectIdentifiers(string lineOfCode);
    
    //Find constants
    void detectConstants(string lineOfCode);
    
    //Find operators
    void detectOperators(string lineOfCode);
    
    //Find delimiters
    void detectDelimiters(string lineOfCode);
    
    //Check if there is imbalance in parenthesis
    bool checkParenthesis(string lineOfCode);
    
    //Check if a keyword is a typo
    bool isTypo(string word);
    
    //Get all the keywords no matter it's a typo or not
    void getKeyWords(string codes);
    
    //Count the nested loop of the program
    void countLoop();
    
};

#endif /* pa3_h */
