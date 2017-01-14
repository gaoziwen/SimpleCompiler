//
//  GrammarRD.hpp
//  compiler
//
//  Created by 高子闻 on 2016/12/25.
//  Copyright © 2016年 高子闻. All rights reserved.
//

#ifndef GrammarRD_hpp
#define GrammarRD_hpp

#include <stdio.h>
#include "Lexical.hpp"

#define MAX_ST 255
#define MAX_RT 255

/* quaternary */
typedef struct
{
    int result;
    int ag1;
    int ag2;
    int op;
} quad;

/* attribute of judgement statement */
typedef struct
{
    int Texit;
    int Fexit;
    int code;
} Econd;

class GrammarRD {
private:
    Lexical *lex;  //Lexical analyzer
    word syn;  //present word
    
    //Semantics
    char symbolT[MAX_ST][128];  //symbol table
    char* opT[11];              //operator table
    quad resultT[MAX_RT];       //quaternary table
    
    int tCount;
    int p_ST;    //pointer of symbol table
    int p_RT;    //pointer of result table
    
    int nextq;  //address of next quaternary
    
public:
    GrammarRD();
    ~GrammarRD();
    
    //Grammar function
    void scaner();  //get another word from lex
    
    void analyze();  //main function
    
    /*VN*/
    void program();
    int block();
    int staString();
    int statement();
    void assignSta();
    int ifSta();
    int whileSta();
    Econd condition();
    int expression();
    int item();
    int factor();
    int rop();
    
    void error(char *info);  //error dealing
    
    
    //Semantics function
    
    void emit(quad q);  //submit quaternary to resultT
    int newTemp();      //create new temp variable, add it to symbolT
    int lookup(char* c);//check if c in symbolT
    int addS(char* c);  //add c to symbolT
    
    void backpatch(int p, int t);
    int merge(int p1, int p2);
    
    void printCode();//print intermediate code
};

#endif /* GrammarRD_hpp */
