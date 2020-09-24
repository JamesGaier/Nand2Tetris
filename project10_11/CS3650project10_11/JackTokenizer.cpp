#include"JackTokenizer.h"
#include"typedefs.h"
#include<fstream>
#include<sstream>
#include<iostream>
#include <string>
#include<climits>
#include<algorithm>
lexer::lexer(const std::string& file){
    std::ifstream input_file{file};
    if(!input_file) {
        std::cout << "File not found in directory" << std::endl;
        return;
    }
    std::stringstream ss;
    std::string line;
    while(std::getline(input_file, line)) {
        ss << line << '\n';
    }
    source = ss.str();
    out_path = file.substr(0,file.rfind('.')) + "T.xml";
}

bool lexer::has_more_tokens() {
    return idx < source.length()-1;
}
void lexer::advance(){
    idx++;
}
char lexer::symbol() {
    return source[idx];
}
std::string lexer::identifier() {
    return source.substr(idx,restart-idx);
}
tds::i16 lexer::int_val() {
    return static_cast<tds::u16>(stoi(source.substr(idx,restart-idx)));
}
std::string lexer::string_val() {
     return source.substr(idx+1,(restart-idx)-2);
}
void lexer::comment() {
    idx = restart;
    if(source[idx] == '/') {
        if(source[idx+1] == '/') {
            while(source[restart] != '\n') {
                restart++;
            }
            idx = restart;
        }
        else if(source[idx+1] == '*') {
            while(source[restart] != '*' || source[restart+1] != '/') {
                restart++;
            }
            restart += 2;
            idx = restart;
        }
    }
    restart = idx;
}
void lexer::white_space() {
    idx = restart;
    while(isspace(source[idx])) {
        idx++;
    }
    restart = idx;
}
tds::token_type lexer::token_type() {
    decltype(idx)old_idx;
    do {
        old_idx = idx;
        white_space();
        comment();
    } while(idx != old_idx);
    if(isdigit(source[idx])) {
        while(isdigit(source[restart])){
            restart++;
        }
        return tds::token_type::integer_constant;
    }
    else if(source[idx] == '"') {
        restart++;
        while(source[restart] != '\n' && source[restart] != '"') restart++;

        if(source[restart] == '\n') {
            std::cout << "found newline in string constant" << std::endl;
        }
        else {
            restart++;
        }
        return tds::token_type::string_constant;
    }
    else if(isalpha(source[idx]) || source[idx] == '_') {
        while(isalnum(source[restart]) || source[restart] == '_') {
            restart++;
        }
        if(keyword_map.count(source.substr(idx, restart-idx)) != 0){
            return tds::token_type::keyword;
        }
        else{
            return tds::token_type::identifier;
        }
    }
    else {
        restart++;
        return tds::token_type::symbol;
    }
}
std::pair<tds::keywords,std::string> lexer::key_word() {
    return std::make_pair(keyword_map.at(source.substr(idx,restart-idx)),
                            source.substr(idx,restart-idx));
}
bool lexer::is_symbol(const char c) {
    return symbols.count(c) != 0;
}