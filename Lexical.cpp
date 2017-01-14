//
//  Lexical.cpp
//  compiler
//
//  Created by 高子闻 on 2016/11/28.
//  Copyright © 2016年 高子闻. All rights reserved.
//

#include "Lexical.hpp"

string Lexical::kw_tab[127] = {"main", "int", "float", "double", "char", "if", "else", "do", "while", KW_END};

Lexical::Lexical(char* filename)
{
    this->file = fopen(filename, "rb");
}

Lexical::~Lexical()
{
    fclose(this->file);
}

void Lexical::getch()
{
    this->ch = fgetc(this->file);
}

void Lexical::getbc()
{
    while ((this->ch == '\n') || (this->ch == '\t') || (this->ch == ' '))
    {
        getch();
    }
}

void Lexical::concat()
{
    this->token += this->ch;
}

bool Lexical::letter(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return true;
    return false;
}

bool Lexical::digit(char c)
{
    if (c >= '0' && c <= '9')
        return true;
    return false;
}

int Lexical::reserve()
{
    int i;
    i = 0;
    string str = Lexical::kw_tab[i];
    while (str != KW_END)
    {
        if (str == this->token)
            return i+1;
        i++;
        str = Lexical::kw_tab[i];
    }
    return 10;
}

void Lexical::retract()
{
    fseek(this->file, -1, SEEK_CUR);
}

double Lexical::dtb()
{
    bool point = false;
    double pm;
    int digit = 0;
    double num = 0.0f;
    int i = 0;
    
    if (token[i] == '+')
    {
        pm = 1;
        i++;
    }
    else if (token[i] == '-')
    {
        pm = -1;
        i++;
    }
    else
    {
        pm = 1;
    }
    
    while (i < token.length())
    {
        if (token[i] == 'e')
        {
            i++;
            break;
        }
        if (token[i] == '.')
        {
            i++;
            point = true;
            break;
        }
        num = num * 10;
        num = num + token[i] - '0';
        i++;
    }
    
    if (point)
    {
        double decimal = 0.1;
        while (i < token.length())
        {
            if (token[i] == 'e')
            {
                i++;
                break;
            }
            
            num = num + decimal * (token[i] - '0');
            decimal = decimal / 10;
            i++;
        }
    }
    
    if (pm == -1)
        num = -num;
    
    if (i < token.length())
    {
        if (token[i] == '+')
        {
            pm = 10;
            i++;
        }
        else if (token[i] == '-')
        {
            pm = 0.1;
            i++;
        }
        else
        {
            pm = 10;
        }
    }
    
    while (i < token.length())
    {
        digit = digit * 10;
        digit = digit + token[i] - '0';
        i++;
    }
    
    for (i = 0; i < digit; i++) {
        num = num * pm;
    }
    
    return num;
}

word Lexical::scaner()
{
    word s;
    s.type = 0;
    s.content.str = "";
    s.content.d = 0.0f;
    
    this->token = "";
    getch();
    getbc();
    
    if (this->ch == EOF)
    {
        return s;//end return 0
    }
    
    if (letter(this->ch))//key words or other words
    {
        while (letter(this->ch) || digit(this->ch))
        {
            concat();
            getch();
        }
        
        retract();
        
        int c = reserve();
        if (c < 10 && c > 0)
        {
            s.type = c;
            s.content.str = (char*)this->token.c_str();
            return s;
        }
        else
        {
            s.type = 10;
            s.content.str = (char*)this->token.c_str();
            return s;
        }
    }
    else if (this->ch == '+' || this->ch == '-' || digit(this->ch))//number
    {
        if (this->ch == '+' || this->ch == '-')
        {
            char temp;
            temp = this->ch;
            
            getch();
            if (digit(this->ch))
            {
                int pre = 0;
                pre = pre_word.type;
                if (pre == 10 || pre == 20 || pre == 27)
                {
                    retract();
                    if (temp == '+')
                    {
                        s.type = 22;
                        s.content.str = "+";
                        return s;
                    }
                    else
                    {
                        s.type = 23;
                        s.content.str = "-";
                        return s;
                    }
                }
            }
            retract();
        }
        retract();
        getch();
        
        int status = 0;
        int pre_status = 0;
        
        status = num_f(status, this->ch);
        
        while (status != -1)
        {
            concat();
            getch();
            pre_status = status;
            status = num_f(status, this->ch);
        }
        
        retract();
        
        if (pre_status == 1)
        {
            if (this->token == "+")
            {
                s.type = 22;
                s.content.str = "+";
                return s;
            }
            else
            {
                s.type = 23;
                s.content.str = "-";
                return s;
            }
        }
        else if (pre_status == 2 || pre_status == 4 ||pre_status == 7)
        {
            s.type = 20;
            s.content.d = dtb();
            return s;
        }
    }
    else//other signal
    {
        switch (this->ch) {
            case '=':
                getch();
                if (this->ch == '=')
                {
                    s.type = 36;
                    s.content.str = "==";
                    return s;
                }
                retract();
                s.type = 21;
                s.content.str = "=";
                return s;
                break;
            case '*':
                s.type = 24;
                s.content.str = "*";
                return s;
                break;
            case '/':
                getch();
                if (this->ch == '*')
                {
                    while (true)
                    {
                        getch();
                        if (this->ch == '*')
                        {
                            getch();
                            if (this->ch == '/')
                            {
                                s.type = 1000;
                                return s;//comment return 1000
                            }
                            retract();
                        }
                    }
                }
                retract();                          //new add, testing
                s.type = 25;
                s.content.str = "/";
                return s;
                break;
            case '(':
                s.type = 26;
                s.content.str = "(";
                return s;
                break;
            case ')':
                s.type = 27;
                s.content.str = ")";
                return s;
                break;
            case '{':
                s.type = 28;
                s.content.str = "{";
                return s;
                break;
            case '}':
                s.type = 29;
                s.content.str = "}";
                return s;
                break;
            case ',':
                s.type = 30;
                s.content.str = ",";
                return s;
                break;
            case ';':
                s.type = 31;
                s.content.str = ";";
                return s;
                break;
            case '>':
                getch();
                if (this->ch == '=')
                {
                    s.type = 33;
                    s.content.str = ">=";
                    return s;
                }
                retract();
                s.type = 32;
                s.content.str = ">";
                return s;
                break;
            case '<':
                getch();
                if (this->ch == '=')
                {
                    s.type = 35;
                    s.content.str = "<=";
                    return s;
                }
                retract();
                s.type = 34;
                s.content.str = "<";
                return s;
                break;
            case '!':
                getch();
                if (this->ch == '=')
                {
                    s.type = 37;
                    s.content.str = "!=";
                    return s;
                }
                retract();
                break;
            default:
                retract();                          //new add, testing
                break;
        }
    }
    s.type = -1;
    error();
    return s;//error return -1
}

int Lexical::num_f(int s, char c)
{
    if (s == 0)
    {
        if (c == '+')
            return 1;
        else if (c == '-')
            return 1;
        else if (digit(c))
            return 2;
    }
    else if (s == 1)
    {
        if (digit(c))
            return 2;
    }
    else if (s == 2)
    {
        if (digit(c))
            return 2;
        else if (c == '.')
            return 3;
        else if (c == 'e')
            return 5;
    }
    else if (s == 3)
    {
        if (digit(c))
            return 4;
    }
    else if (s == 4)
    {
        if (digit(c))
            return 4;
        else if (c == 'e')
            return 5;
    }
    else if (s == 5)
    {
        if (c == '+')
            return 6;
        else if (c == '-')
            return 6;
        else if (digit(c))
            return 7;
    }
    else if (s == 6)
    {
        if (digit(c))
            return 7;
    }
    else if (s == 7)
    {
        if (digit(c))
            return 7;
    }
    return -1;
}

void Lexical::error()
{
    printf("Error happened when lexical analysis!\n");
}

void Lexical::init()
{
    word_now.type = 0;
    word_now.content.str = "";
    word_now.content.d = 0;
}

word Lexical::next()
{
    while (true) {
        this->pre_word = word_now;
        word_now = scaner();
        if (word_now.type == 1000)
            continue;
        if (word_now.type == -1)
            continue;
        
        if (word_now.type == 20)
            printf("(%d, %g)\n", word_now.type, word_now.content.d);
        else
            printf("(%d, %s)\n", word_now.type, word_now.content.str);
        
        return word_now;
    }
}

word* Lexical::test()
{
    //this->point = 0;
    
    word s;
    s.type = 0;
    s.content.str = "";
    
    while (true)
    {
        this->pre_word = s;
        s = scaner();
        if (s.type == 1000)
            continue;
        if (s.type == 0)
            break;
        if (s.type == -1)
            continue;
        
        if (s.type == 20)
            printf("(%d, %g)\n", s.type, s.content.d);
        else
            printf("(%d, %s)\n", s.type, s.content.str);
        //this->output[this->point] = s;
        //this->point++;
    }
    
    //this->len = this->point;
    return this->output;
}

int Lexical::getLen()
{
    return this->len;
}

void Lexical::test2()
{
    this->token = "-1.345e+20";
    
    printf("%ef\n", dtb());
}
