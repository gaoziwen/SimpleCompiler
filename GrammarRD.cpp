//
//  GrammarRD.cpp
//  compiler
//
//  Created by 高子闻 on 2016/12/25.
//  Copyright © 2016年 高子闻. All rights reserved.
//

#include "GrammarRD.hpp"

GrammarRD::GrammarRD()
{
    lex = new Lexical("./input.txt");
    
    lex->init();
    
    opT[0] = "+";
    opT[1] = "-";
    opT[2] = "*";
    opT[3] = "/";
    opT[4] = ">";
    opT[5] = ">=";
    opT[6] = "<";
    opT[7] = "<=";
    opT[8] = "==";
    opT[9] = "!=";
    opT[10] = "";
}

GrammarRD::~GrammarRD()
{
    
}

void GrammarRD::scaner()
{
    this->syn = lex->next();
}

void GrammarRD::analyze()
{
    nextq = 100;
    
    tCount = 0;
    p_ST = 0;
    p_RT = 0;
    scaner();
    program();
    if (syn.type == 0)
        printf("Analysing complete!\n");
    else
        printf("Fail!\n");
}

void GrammarRD::error(char *info)
{
    printf("Grammar Error : %s!\n", info);
}

void GrammarRD::program()
{
    int ret;
    
    if (syn.type == 1)
    {
        scaner();
        if (syn.type == 26)
        {
            scaner();
            if (syn.type == 27)
            {
                scaner();
                ret = block();
            }
            else
                error("Not found \')\'");
        }
        else
            error("Not found \'(\'");
    }
    else
        error("Not found main function");
    
}

int GrammarRD::block()
{
    int ret;
    if (syn.type == 28)
    {
        scaner();
        ret = staString();
        if (syn.type == 29)
        {
            scaner();
        }
        else
            error("Not found \'}\'");
    }
    else
        error("Not found \'{\'");
    
    return ret;
}


int GrammarRD::staString()
{
    int ret;
    
    ret = statement();
    if (ret != -1)
        backpatch(ret, nextq);
    while (syn.type == 6 || syn.type == 8 || syn.type == 10)
    {
        ret = statement();
        if (ret != -1)
            backpatch(ret, nextq);
    }
    
    return ret;
}

int GrammarRD::statement()
{
    int ret = -1;
    
    if (syn.type == 10)
    {
        ret = -1; //assign -1
        assignSta();
        if (syn.type == 31)
        {
            scaner();
        }
        else
            error("Not found \';\'");
    }
    else if (syn.type == 6)
    {
        ret = ifSta();
    }
    else if (syn.type == 8)
    {
        ret = whileSta();
        if (syn.type == 31)
        {
            scaner();
        }
        else
            error("Not found \';\'");
    }
    else
        error("Statement wrong");
    
    return ret;
}

void GrammarRD::assignSta()
{
    int ret, ag1;
    quad q;
    
    if (syn.type == 10)
    {
        int p = lookup(syn.content.str);
        if (p != -1)
        {
            ret = p;
        }
        else
        {
            ret = addS(syn.content.str);
        }
        
        scaner();
        if (syn.type == 21)
        {
            scaner();
            ag1 = expression();
            
            q.result = ret;
            q.ag1 = ag1;
            q.op = 10;
            q.ag2 = 0;
            emit(q);
        }
        else
            error("Not found \'=\'");
    }
    else
        error("Assign statement wrong");
}

int GrammarRD::ifSta()
{
    Econd e;
    int cChain;
    int sChain;
    int s1Chain;//undone
    int tpChain;
    int s2Chain;//undone
    
    if (syn.type == 6)
    {
        scaner();
        e = condition();
        
        backpatch(e.Texit, nextq);
        cChain = e.Fexit;
        
        s1Chain = block();
        
        if (syn.type == 7)
        {
            int q = nextq;     //question
            
            quad qu;
            qu.op = 11;
            qu.result = 0;
            qu.ag1 = 0;
            qu.ag2 = 0;
            emit(qu);
            
            backpatch(cChain, nextq);
            
            if (s1Chain != -1)
                tpChain = merge(q, s1Chain);
            else
                tpChain = q;
            
            scaner();
            s2Chain = block();
            
            if (s2Chain != -1)
                sChain = merge(s2Chain, tpChain);
            else
                sChain = tpChain;
        }
        else
        {
            //backpatch(sChain, nextq);
            if (s1Chain != -1)
                sChain = merge(s1Chain, cChain);
            else
                sChain = cChain;
        }
    }
    else
        error("If statement wrong");
    
    return sChain;
}

int GrammarRD::whileSta()
{
    int q;
    Econd e;
    if (syn.type == 8)
    {
        scaner();
        q = nextq;
        block();
        if (syn.type == 9)
        {
            scaner();
            e = condition();
            
            resultT[e.Texit - 100].result = q;
            
        }
        else
            error("Not found \'while\'");
    }
    else
        error("While statement wrong");
    
    return e.Fexit;
}

Econd GrammarRD::condition()
{
    int ret, ag1, op, ag2;
    quad q;
    Econd e;
    
    ag1 = expression();
    op = rop();
    ag2 = expression();
    
    e.Texit = nextq;
    e.code = nextq;
    e.Fexit = nextq + 1;
    
    q.ag1 = ag1;
    q.ag2 = ag2;
    q.op = op;
    q.result = 0;
    emit(q);
    
    q.ag1 = 0;
    q.ag2 = 0;
    q.op = 11;
    q.result = 0;
    emit(q);
    
    return e;
}

int GrammarRD::expression()
{
    int result,ag1,ag2,op;
    quad q;
    
    ag1 = item();
    while(syn.type == 22 || syn.type == 23)
    {
        op = syn.type;
        scaner();
        ag2 = item();
        result = newTemp();
        
        q.ag1 = ag1;
        q.ag2 = ag2;
        q.result = result;
        q.op = op;
        
        emit(q);
        
        ag1 = result;
    }
    
    return ag1;
}

int GrammarRD::item()
{
    int result,ag1,ag2,op;
    quad q;
    
    ag1 = factor();
    while (syn.type == 24 || syn.type == 25)
    {
        op = syn.type;
        scaner();
        ag2 = factor();
        result = newTemp();
        
        q.ag1 = ag1;
        q.ag2 = ag2;
        q.result = result;
        q.op = op;
        emit(q);
        
        ag1 = result;
    }
    
    return ag1;
}

int GrammarRD::factor()
{
    int ret;
    
    if (syn.type == 10)
    {
        int p = lookup(syn.content.str);
        if (p != -1)
        {
            ret = p;
        }
        else
        {
            ret = addS(syn.content.str);
        }
        
        scaner();
    }
    else if (syn.type == 20)
    {
        char str[30];
        sprintf(str, "%g", syn.content.d);
        ret = addS(str);
        
        scaner();
    }
    else if (syn.type == 26)
    {
        scaner();
        ret = expression();
        if (syn.type == 27)
        {
            scaner();
        }
        else
            error("Not found \')\'");
    }
    else
        error("Factor wrong");
    
    return ret;
}

int GrammarRD::rop()
{
    int ret = syn.type;
    if (syn.type == 32)
    {
        scaner();
    }
    else if (syn.type == 33)
    {
        scaner();
    }
    else if (syn.type == 34)
    {
        scaner();
    }
    else if (syn.type == 35)
    {
        scaner();
    }
    else if (syn.type == 36)
    {
        scaner();
    }
    else if (syn.type == 37)
    {
        scaner();
    }
    else
        error("Relational operator worng");
    return ret;
}

int GrammarRD::newTemp()
{
    char str2[5];
    char str[10] = "T";
    if (p_ST < MAX_ST)
    {
        sprintf(str2 , "%d", tCount);
        strcat(str, str2);
        strcpy(symbolT[p_ST], str);
        p_ST++;
        tCount++;
    }
    return p_ST - 1;
}

int GrammarRD::lookup(char *c)
{
    int i;
    for (i = 0; i < p_ST; i++)
    {
        if (strcmp(symbolT[i], c) == 0)
        {
            return i;
        }
    }
    return -1;
}

int GrammarRD::addS(char *c)
{
    if (p_ST < MAX_ST)
    {
        strcpy(symbolT[p_ST], c);
        p_ST++;
    }
    return p_ST-1;
}

void GrammarRD::emit(quad q)
{
    if (p_RT < MAX_RT)
    {
        resultT[p_RT] = q;
        p_RT++;
        nextq++;
    }
}

void GrammarRD::backpatch(int p, int t)
{
    int next;
    int now = p;
    
    if (p == -1 || t == -1)
        return;
    
    do
    {
        next = resultT[now - 100].result;
        resultT[now - 100].result = t;
        now = next;
    }
    while (next != 0);
}

int GrammarRD::merge(int p1, int p2)
{
    int now = p1;
    
    while (resultT[now - 100].result != 0)
    {
        now = resultT[now - 100].result;
    }
    resultT[now - 100].result = p2;
    
    return p1;
}

void GrammarRD::printCode()
{
    int i;
    int op, r, ag1, ag2;
    
    for(i = 0; i < p_RT; i++)
    {
        op = resultT[i].op;
        r = resultT[i].result;
        ag1 = resultT[i].ag1;
        ag2 = resultT[i].ag2;
        if (op >= 22 && op <=25)
        {
            printf("%d: %s = %s %s %s\n", i + 100, symbolT[r], symbolT[ag1], opT[op - 22], symbolT[ag2]);
        }
        else if (op == 10)
        {
            printf("%d: %s = %s\n", i + 100, symbolT[r], symbolT[ag1]);
        }
        else if (op >= 32 && op <= 37)
        {
            printf("%d: if %s %s %s goto %d\n", i + 100, symbolT[ag1], opT[op - 28], symbolT[ag2], r);
        }
        else if (op == 11)
        {
            printf("%d: goto %d\n", i + 100, r);
        }
        //to be continue
    }
}

