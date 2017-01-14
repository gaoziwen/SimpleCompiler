//
//  main.cpp
//  compiler
//
//  Created by 高子闻 on 2016/11/28.
//  Copyright © 2016年 高子闻. All rights reserved.
//

#include <iostream>
#include "Lexical.hpp"
#include "Grammar.hpp"
#include "GrammarRD.hpp"

using namespace std;

int main(int argc, const char * argv[])
{
    
    GrammarRD* gra = new GrammarRD();  //Create new Grammar Analyzer
    
    gra->analyze();                    //Analyze begin
    
    printf("\n");
    
    gra->printCode();
    
    return 0;
}
