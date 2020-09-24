#include"CompilationEngine.h"
#include"typedefs.h"
#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#include <cstring>
compilation_engine::compilation_engine(const std::string& input_file,
                    const std::string& output_path_): output_path{output_path_} {

    output_file.open(output_path.c_str());
    std::ifstream input{input_file};
    if(!input) {
        std::cout << "Cannot find file in directory" << std::endl;
        return;
    }
    std::stringstream ss;
    std::string line;
    while(getline(input, line)) {
        ss << line;
    }
    input_code = ss.str();

    tokens = get_tokens();
    for(const auto& el: tokens) {
        //std::cout << el.lexeme << std::endl;
    }
}
std::vector<tds::token> compilation_engine::get_tokens() {
    std::stringstream ss;
    auto done = false;
    tds::u32 idx = 0;
    tds::u32 restart = 0;
    std::vector<tds::token> tokens;
    while(input_code[restart] != '>') {
        restart++;
    }
    restart++;
    idx = restart;
    while(idx < input_code.length()) {
        tds::token tok;
        idx = restart;
        while(isspace(input_code[idx])) idx++;
        restart = idx;
        if(input_code[idx] == '<' && input_code[idx+1] == '/') {
            auto temp = restart;
            temp+=2;
            while(input_code[temp] != '>') {
                ss << input_code[temp];
                temp++;
            }
            if(ss.str() == "tokens") {
                break;
            }
            ss.str("");
        }
        if(input_code[idx] == '<') {
            restart++;
            while(input_code[restart] != '>') {
                ss << input_code[restart];
                restart++;
            }
            auto type = ss.str();
            //std::cout << type << std::endl;
            if(type == "keyword") {
                tok.type = tds::token_type::keyword;
            }
            else if(type == "symbol") {
                tok.type = tds::token_type::symbol;
            }
            else if(type == "identifier") {
                tok.type = tds::token_type::identifier;
            }
            else if(type == "integerConstant") {
                tok.type = tds::token_type::integer_constant;
            }
            else if(type == "stringConstant") {
                tok.type = tds::token_type::string_constant;
            }
            restart++;
            idx = restart;
            ss.str("");
        }
        if(input_code[restart] != '<') {
            while(input_code[restart] != '<') {
                ss << input_code[restart++];
            }
            tok.lexeme = ss.str();
            ss.str("");
            idx = restart;
        }
        if(input_code[idx] == '<' && input_code[idx+1] == '/') {
            while(input_code[restart] != '>') restart++;
            restart++;
            idx = restart;
        }
        tokens.push_back(tok);
    }
    return tokens;
}

std::string get_tok_type(tds::token_type type) {
    auto result = "";
    switch(type) {
        case tds::token_type::keyword:
            result = "keyword";
            break;
        case tds::token_type::symbol:
            result = "symbol";
            break;
        case tds::token_type::identifier:
            result = "identifier";
            break;
        case tds::token_type::integer_constant:
            result =  "integerConstant";
            break;
        case tds::token_type::string_constant:
            result =  "stringConstant";
            break;
    }
    return result;
}
void compilation_engine::advance() {
    if(comp_idx < tokens.size()) {
        comp_idx++;
    }
}
void compilation_engine::match(const std::string& expected) {
    if(expected == tokens[comp_idx].lexeme){
        //std::cout << tokens[comp_idx].lexeme << std::endl;
        write_to_file();
        advance();
    }
    else {
        //std::ofstream log{"error.log"};
        //std::cout << comp_idx << tokens.size() << std::endl;
        std::cout << "syntax error expected " << expected << " got "
            << tokens[comp_idx].lexeme << std::endl;
        //std::cout << comp_idx << tokens[comp_idx].lexeme << std::endl;
        //log << "syntax error expected " << expected << " got "
            //<< tokens[comp_idx].lexeme << std::endl;
        //exit(3);
        syntax_error = true;
    }
}
void compilation_engine::write_to_file() {
    std::cout << tokens[comp_idx].lexeme << std::endl;
    output_file  << "<" << get_tok_type(tokens[comp_idx].type)
                    << ">" << tokens[comp_idx].lexeme
                    << "</" << get_tok_type(tokens[comp_idx].type) << ">\n";
}
void compilation_engine::identifier() {
    bool is_identifier = true;
    if(tokens[comp_idx].type != tds::token_type::identifier) {
        is_identifier = false;
    }
    for(auto i = 0; i < tokens[comp_idx].lexeme.length(); i++) {
        if(!isalnum(tokens[comp_idx].lexeme[i]) || tokens[comp_idx].lexeme[i] == '_') {
            is_identifier = false;
        }
    }
    if(is_identifier) {

        write_to_file();
        advance();
    }
    else {
        std::cout << "expected identifier got " << tokens[comp_idx].lexeme << std::endl;
        syntax_error = true;
        //output_file.close();
        //exit(3);
    }
}
bool compilation_engine::is_type() {
    auto is_type = primatives.count(tokens[comp_idx].lexeme) != 0;
    std::ifstream file{tokens[comp_idx].lexeme};
    if(!file && !is_type) {
        //std::cout << "type " << tokens[comp_idx].lexeme << " not found" << std::endl;
        is_type = false;
        //syntax_error = true;
    }
    return is_type;
}
void compilation_engine::type() {
    if(is_type()) {
        write_to_file();
        advance();
    }
    else {
        std::cout << "type not found " << get_tok_type(tokens[comp_idx].type) << std::endl;
    }
}
void compilation_engine::compile_class() {
    output_file << "<class>\n";
    match("class");
    identifier();
    match("{");
    depth++;
    //compile_class_var_dec();
    compile_subroutine();
    match("}");
    output_file << "</class>";
}
void compilation_engine::compile_class_var_dec() {
    output_file << "<classVarDec>";
    if(tokens[comp_idx].lexeme == "static") {
        match("static");
        type();
        identifier();
    }
    else if(tokens[comp_idx].lexeme == "field") {
        match("field");
        type();
        identifier();
    }
    output_file << "</classVarDec>";

}
void compilation_engine::compile_subroutine() {
    output_file << "\t<subroutineDec>\n";

    if(tokens[comp_idx].lexeme == "constructor") {
        match("constructor");
    }
    else if(tokens[comp_idx].lexeme == "function") {
        match("function");
    }
    else if(tokens[comp_idx].lexeme == "method") {
        match("method");
    }
    if(tokens[comp_idx].lexeme == "void") {
        match("void");
        identifier();
        match("(");
        if(tokens[comp_idx].lexeme != ")") {
            //compile_parameter_list();
        }
        match(")");
        compile_subroutine_body();
    }
    else if(is_type()) {
        type();
        identifier();
        match("(");
        if(tokens[comp_idx].lexeme != ")") {
            //compile_parameter_list();
        }
        match(")");
        compile_subroutine_body();
    }
    output_file << "\t</subroutineDec>\n";
}
void compilation_engine::compile_var_list() {
    identifier();
    if(tokens[comp_idx].lexeme == ",") {
        advance();
        compile_var_list();
    }
}
void compilation_engine::compile_var_dec() {
    output_file  << "<varDec>\n";
    match("var");
    type();
    compile_var_list();
    match(";");
    output_file  << "</varDec>\n";
    if(tokens[comp_idx].lexeme == "var") {
        compile_var_dec();
    }
}
void compilation_engine::compile_subroutine_body() {
    match("{");
    depth++;
    compile_var_dec();
    compile_statements();
    match("}");
}
void compilation_engine::compile_statements() {
    if(tokens[comp_idx].lexeme != "}") {
        if(tokens[comp_idx].lexeme == "do") {
            //compile_do();
        }
        else if(tokens[comp_idx].lexeme == "let") {
            compile_let();
        }
        else if(tokens[comp_idx].lexeme == "while") {
            compile_while();
        }
        else if(tokens[comp_idx].lexeme == "return") {
            //compile_return();
        }
        else if(tokens[comp_idx].lexeme == "if") {
            //compile_if();
        }
        compile_statements();
    }
}
void compilation_engine::compile_let() {
    output_file <<  "<letStatement>\n";
    match("let");
    identifier();
    if(tokens[comp_idx].lexeme == "[") {
        match("[");
        compile_expression();
        match("]");
    }
    match("=");
    compile_expression();
    match(";");
    output_file <<  "</letStatement>\n";
}
bool compilation_engine::is_operator() {
    return operators.count(tokens[comp_idx].lexeme) != 0;
}
void compilation_engine::consume_op() {
    auto op_found = false;
    std::string oper = "";
    for(const auto& op: operators) {
        if(op == tokens[comp_idx].lexeme) {
            op_found = true;
            oper = tokens[comp_idx].lexeme;
        }
    }
    if(op_found) {
        write_to_file();
        advance();
    }
}
void compilation_engine::compile_expression() {
    output_file  << "<expression>\n";
    compile_term();
    if(is_operator()) {
        consume_op();
        compile_term();
        compile_expression();
    }
    output_file  << "</expression>\n";

}
void compilation_engine::compile_term() {
    output_file  << "<term>\n";
    if(tokens[comp_idx].type == tds::token_type::integer_constant) {
        match(tokens[comp_idx].lexeme);
        output_file  << "</term>\n";
    }
    else if(tokens[comp_idx].type == tds::token_type::string_constant) {
        match(tokens[comp_idx].lexeme);
        output_file  << "</term>\n";
    }
    else if(tokens[comp_idx].type == tds::token_type::keyword) {
        match(tokens[comp_idx].lexeme);
        output_file  << "</term>\n";
    }
    else if(tokens[comp_idx].lexeme == "(") {
        match("(");
        compile_expression();
        match(")");
        output_file  << "</term>\n";
    }
    else if(tokens[comp_idx].type == tds::token_type::identifier) {
        identifier();
        if(tokens[comp_idx].lexeme == "(") {
            match("(");
            if(tokens[comp_idx].lexeme != ")") {
                compile_expression_list();
            }
            match(")");
        }
        else if(tokens[comp_idx].lexeme == ".") {
            match(".");
            identifier();
            match("(");
            if(tokens[comp_idx].lexeme != ")") {
                compile_expression_list();
            }
            match(")");
        }
    }
    else if(is_type()) {
        match(".");
        identifier();
        match("(");
        if(tokens[comp_idx].lexeme != ")") {
            compile_expression_list();
        }
        match(")");
    }
    /*
    else if(unaryOp) {
        match(unaryOp);
        compile_term();
    }
    */

}
void compilation_engine::compile_expression_list() {
    output_file <<  "<expressionList>\n";
    compile_expression();
    if(tokens[comp_idx].lexeme == ",") {
        match(",");
        compile_expression();
        compile_expression_list();
    }
    output_file <<  "</expressionList>\n";
}
void compilation_engine::compile_while() {
    match("while");
    match("(");
    compile_expression();
    match(")");
    match("{");
    compile_statements();
    match("}");
}
/*
void compilation_engine::compile_subroutine_call() {

}
void compilation_engine::compile_parameter_list() {
    if(peek() == ")") {
        return;
    }
    match(type);
    match(identifier);
    if(peek() == ",") {
        compile_parameter_list();
    }
}

void compilation_engine::compile_do() {
    match("do");
    compile_subroutine_call();
}

void compilation_engine::compile_return() {
    match("return");
    if(peek() != ";") {
        compile_expression();
        match(";");
    }
    else{
        match(";");
    }
}
void compilation_engine::compile_if() {
    match("if");
    match("(");
    compile_expression();
    match(")");
    match("{");
    compile_statements();
    match("}");
    if(peek() == "else") {
        match("else");
        match("{");
        compile_statements();
        match("}");
    }
}


*/