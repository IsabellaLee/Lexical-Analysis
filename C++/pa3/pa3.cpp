//
//  main.cpp
//  ProjectAssign#3
//
//  Created by Lin Li on 3/29/17.
//  Copyright © 2017 Isabella Lee. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "pa3.hpp"

using namespace std;

int main() {
   cout << "INPUT> Please enter the name of the input file:\n";
    string fileName; //fileName is the name of .txt file that will be opened
    cin >> fileName;
    cout << endl;
   
    vector<string> codes; //codes are the content of .txt file
   
    
    ifstream codefile(fileName);
    string line;
    
    //push each line of file into codes
    if (codefile) {
        while (getline(codefile, line)) {
          codes.push_back(line);
        }
    } else {
        cout << "Unable to find file" << endl;
        exit(0);
    }
    
    //construct a compiler
    
    Compiler compiler(codes);
    compiler.read();
  
    cout << "OUTPUT> The depth of nested loop(s) is " << compiler.loops << endl;
    cout << endl;
    
    cout << "Keywords: ";
    //print out the keywords in the codes
    compiler.printMap(compiler.keywords);

    cout << "Identifier: ";
    //print out the identifiers in the codes
    compiler.printMap(compiler.identifiers);

    cout << "Constant: ";
    //print out the constants in the codes
    compiler.printMap(compiler.constants);
    
    cout << "Operators: ";
    //print out the operators in the codes
    compiler.printMap(compiler.operators);

    cout << "Delimiter: ";
    //print out the delimiters in the codes
    compiler.printMap(compiler.delimiters);
    cout << endl;

    cout << "Syntax Error(s): ";
    //print out the errors in the codes
    compiler.printSyntaxErrors();
    cout << endl;
    
    return 0;
    
}

Compiler::Compiler(vector<string> codes) {
    this->codes = codes;
}

void Compiler::read() {
	int cnt = 0;
    for (std::vector<string>::iterator it = codes.begin() ; it != codes.end(); ++it) {
        string lineOfCode = *it; //Each line is a string
        
        detectKeyWords(lineOfCode);
        detectDelimiters(lineOfCode);
        detectIdentifiers(lineOfCode);
        detectOperators(lineOfCode);
        detectConstants(lineOfCode);
        bool check = checkParenthesis(lineOfCode);
        if (!check) cnt++;
        
        getKeyWords(lineOfCode);
    }
    
    //Calculate the loops based on only keywords
    countLoop();
    
    //Check the situation where a missing , or ; will invalid a for loop
    if (loops >= 1) {
        //Test case for first if statement, since error will only be in the inner-most loop,
        //I simplified the test process here to only test if the numbers of , and ; match
        /*
         FOR (i, 10, ++)
         BEGIN
         FOR (j, 10, ++)
         BEGIN
         sum=sum + i + j
         END
         */
        
        if (delimiters[','] < 2 * loops && delimiters.find(';') == delimiters.end()) {
            syntaxErrors.push_back(",");
            syntaxErrors.push_back(";");
            loops--;
        }
        //Test case for next else if statement
        /*
         FOR (i, 10, ++)
         BEGIN
         FOR (j 10, ++)
         BEGIN
         sum=sum + i + j;
         END
         END
         */
        else if(delimiters[','] < 2 * loops && delimiters.find(';') != delimiters.end()){
              syntaxErrors.push_back(",");
              loops--;
        }
        //Test case for next else if statement
        /*
         FOR (i, 10, ++)
         BEGIN
         FOR (j, 10, ++)
         BEGIN
         sum=sum + i + j
         END
         END
         */

        else if (delimiters[','] == 2 * loops && delimiters.find(';') == delimiters.end()){
            syntaxErrors.push_back(";");
            loops--;
        }
    }
    
    //Test based on () mismatch
    if (loops > 0) {
		if (cnt > 0)
			loops--;
	}
}

void Compiler::printSyntaxErrors(){
    if (syntaxErrors.empty()) {
        cout << "NA" << endl;
        return;
    }

    for (vector<string>::iterator it = syntaxErrors.begin(); it != syntaxErrors.end(); ++it) {
		cout << *it << " ";
    }
    cout << endl;
}

void Compiler::printMap (unordered_map<string, int> map) {
    for (unordered_map<string, int> :: iterator it = map.begin(); it != map.end(); ++it) {
        cout << it->first << " ";
    }
    cout << endl;
}

void Compiler::printMap (unordered_map<char, int> map) {
    for (unordered_map<char, int> :: iterator it = map.begin(); it != map.end(); ++it) {
        cout << it->first << " ";
    }
    cout << endl;
}

void Compiler::detectKeyWords(string lineOfCode) {
    
    //If this line of code contains a keyword, it will be added to
    //its corresponding map
    
    if (lineOfCode.find("BEGIN") != string::npos) {
        if (keywords["BEGIN"] < 1)
            keywords["BEGIN"] += 1;
    }
    if (lineOfCode.find("END") != string::npos) {
        if (keywords["END"] < 1)
            keywords["END"] += 1;
    }
    if (lineOfCode.find("FOR") != string::npos) {
        if (keywords["FOR"] < 1)
            keywords["FOR"] += 1;
    }
}

void Compiler::detectIdentifiers(string lineOfCode) {
    deque<char> dqOfIdentifiers;
    for (unsigned i = 0; i < lineOfCode.length(); i++) {
        char character = lineOfCode[i];
        if (islower(character)) {
            dqOfIdentifiers.push_back(character);
        } else {
            string indentifier;
            while (!dqOfIdentifiers.empty()) {
                indentifier += dqOfIdentifiers.front();
                dqOfIdentifiers.pop_front();
            }
            if (identifiers[indentifier] != 1)
                identifiers[indentifier] = 1;
        }
        
    }
}

void Compiler::detectConstants(string lineOfCode) {
    deque<char> dqOfConstant;
    
    for (unsigned i = 0; i < lineOfCode.length(); i++) {
        if (isdigit(lineOfCode[i])) {
            dqOfConstant.push_back(lineOfCode[i]);
        } else {
            if (dqOfConstant.empty()) continue;
            string constant;
            while (!dqOfConstant.empty()) {
                constant += dqOfConstant.front();
                dqOfConstant.pop_front();
            }
            if (constants[constant] != 1)
                constants[constant] = 1;
        }
    }
}

void Compiler::detectOperators(string lineOfCode) {
    deque<char> dqOfOperators;
    for (unsigned i = 0; i < lineOfCode.length(); i++) {
        char character = lineOfCode[i];
        if (character == '+' || character =='-' || character == '*' || character =='/' || character == '=')
            dqOfOperators.push_back(character);
        else {
            if (dqOfOperators.empty()) continue;
            string ops;
            while (!dqOfOperators.empty()) {
                ops += dqOfOperators.front();
                dqOfOperators.pop_front();
            }
            if (operators[ops] != 1)
                operators[ops] = 1;
        }
    }
}

void Compiler::detectDelimiters(string lineOfCode) {
  
    for (unsigned i = 0; i < lineOfCode.length(); i++) {
        char character = lineOfCode[i];
        if (character == ',' || character ==';') {
            auto it = delimiters.find(character);
            if (it != delimiters.end()) {
                it->second = delimiters[character] + 1;
            } else {
                delimiters[character] = 1;
            }
        }
    }
}

bool Compiler::checkParenthesis(string lineOfCode) {
    deque<char> dq;
    int count = 0;
    int right = 0;

  
    for (unsigned i = 0; i < lineOfCode.length(); i++) {
        if (lineOfCode[i] == '(') {
           dq.push_back(lineOfCode[i]); 
        }
        if (lineOfCode[i] == ')') {
			
           if (dq.empty()) {
			   if (right > 0) {
				syntaxErrors.push_back(")");
				count++;
			   } else {
				   syntaxErrors.push_back("(");
			   }
			}
            else {
				dq.pop_back();
				right++;
			}
        }
    }
    

    if (!dq.empty() && right > 0) {
       syntaxErrors.push_back("(");
        count++;
   }
	else if (!dq.empty() && right == 0) {
       syntaxErrors.push_back(")");
       count++;
   }
   
   if (count > 0) return false;
   return true;
}

bool Compiler::isTypo(string word) {
    if (word != "BEGIN" && word != "END" && word != "FOR")
        return true;
    else
        return false;
}

void Compiler::getKeyWords(string codes) {
    deque<char> dqOfWords;
    
    for (unsigned i = 0; i < codes.length(); i++) {
        if (isupper(codes[i])) {
           dqOfWords.push_back(codes[i]);
        } 
        else {
            if (dqOfWords.empty()) 
				continue;
          string word;
            while (!dqOfWords.empty()) {
               word += dqOfWords.front();
                  dqOfWords.pop_front();
            }
			 words.push_back(word);
		}
    }
   
    if (dqOfWords.empty()) return;
    string word;
    while (!dqOfWords.empty()) {
        word += dqOfWords.front();
        dqOfWords.pop_front();
    }
    words.push_back(word);
    
}

void Compiler::countLoop() {
	deque<string> dq;
	while (words.size() > 0) {
		
		while (words.front() != "END" && words.size() > 0) {
			string temp = words.front();
            if (isTypo(temp)) syntaxErrors.push_back(temp);
            dq.push_back(temp);
			//words.pop_front();
			words.erase(words.begin());
		}
		
        int current = 0;
        
		while (words.front() == "END" && words.size() > 0) {
			//words.pop_front();
           
			words.erase(words.begin());
			
			
            if (!dq.empty()) {
                string temp = dq.back();
                if (isTypo(temp) || temp != "BEGIN") {
					
					if (temp == "FOR")
						syntaxErrors.push_back("BEGIN");
						
                 /*   {
						if (!dq.empty())
							dq.pop_back();
                    } while (dq.back() != "FOR") */
                  
                    dq.pop_back();
                   if (!words.empty()) {
                        if (dq.back() == "BEGIN") {
							dq.pop_back();
							if (dq.back() == "FOR") {
								current++;
								if (current > loops) loops = current;
							}
						}
						
                        if (current > loops) loops = current;
                        dq.pop_back();

                    }
                } else {
                    dq.pop_back();
                    string back = dq.back();
                    if (back == "FOR") {
                        current++;
                      //  cout << current << endl;
                        if (current > loops) loops = current;

                    } else {
                        syntaxErrors.push_back("FOR");
                    }
                    dq.pop_back();
                }
            }
		}
        
	}
    if (!dq.empty()) syntaxErrors.push_back("END");

}
	
