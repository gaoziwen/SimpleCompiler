//
//  Lexical.hpp
//  compiler
//
//  Created by 高子闻 on 2016/11/28.
//  Copyright © 2016年 高子闻. All rights reserved.
//

#ifndef Lexical_hpp
#define Lexical_hpp

#include <stdio.h>
#include <string>
#include <iostream>

#define KW_END "KW_END_WORD"

using namespace std;

typedef  union {
    char* str;
    double d;
} Cont;

typedef struct {
    int type;
    Cont content;
} word;


class Lexical {
private:
    static string kw_tab[127];  //keyword table
    
    word pre_word;  //pervious input word
    
    word word_now;  //analyzing word
    
    word output[2048]; //tuple list
    int point;
    int len;

    char ch;  //next character
    int p_token;  //point of token
    string token;  //string wating for lexical analyzing
    FILE *file;  //file pointer
    
public:
    Lexical(char* filename);
    ~Lexical();
    void getch();  //get next character,put it into ch
    void getbc();  //jump over every space, tab, enter
    void concat();  //add ch into token
    bool letter(char c);  //if c is letter
    bool digit(char c);  //if c is number
    int reserve();  //check if token is a keyword, if not, return 10
    void retract();  //file pointer - 1
    double dtb();  //convert token to a double number
    word scaner();  //main function
    int num_f(int s, char c);  //a DFA for judging if token is a number
    void error();  //error dealing
    word* test();  //Lexical tester
    void test2();  //dtb() tester
    int getLen();  //return this->len
    
    //For GrammarRD
    word next();  //get next word
    void init();  //initialize the Lexical Analyzer for GrammarRD
};

#endif /* Lexical_hpp */
