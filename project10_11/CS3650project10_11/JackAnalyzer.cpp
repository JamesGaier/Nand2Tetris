#include"JackTokenizer.h"
#include"CompilationEngine.h"
#include"CompilationEngine.h"
#include<fstream>
#include<iostream>
#include<string>
bool is_special(const char symbol) {
    return symbol == '&' || symbol == '<'
    || symbol == '>' || symbol == '"';
}
void run(lexer & lex) {
    std::ofstream output_file{lex.output_path()};
    output_file << "<tokens>\n";
    while(lex.has_more_tokens()) {

        switch(lex.token_type()) {
            case tds::token_type::keyword:{
                auto keyword = lex.key_word();
                output_file << "\t<keyword>" << keyword.second << "</keyword>\n";
                break;
            }
            case tds::token_type::symbol:
                if(lex.is_symbol(lex.symbol()) && !is_special(lex.symbol())){
                    output_file << "\t<symbol>" << lex.symbol() << "</symbol>\n";
                }
                else if(is_special(lex.symbol())) {
                    switch(lex.symbol()) {
                        case '&':
                            output_file << "\t<symbol>" << "&amp;" << "</symbol>\n";
                            break;
                        case '<':
                            output_file << "\t<symbol>" << "&lt;" << "</symbol>\n";
                            break;
                        case '>':
                            output_file << "\t<symbol>" << "&gt;" << "</symbol>\n";
                            break;
                        case '"':
                            output_file << "\t<symbol>" << "&quot;" << "</symbol>\n";
                            break;
                        default:
                            std::cout << "error, how did you get here?" << lex.symbol() << std::endl;
                            break;
                    }
                }
                else if(lex.symbol() == '&') {

                }
                else if(lex.symbol() != 0)
                    std::cout << "error bad token" << std::endl;
                break;
            case tds::token_type::string_constant:
                output_file << "\t<stringConstant>" << lex.string_val() << "</stringConstant>\n";
                break;
            case tds::token_type::identifier:
                //std::cout << lex.identifier() << std::endl;
                output_file << "\t<identifier>" << lex.identifier() << "</identifier>\n";
                break;
            case tds::token_type::integer_constant:
                output_file << "\t<integerConstant>" << lex.int_val() << "</integerConstant>\n";
                break;
            default:
                break;
        }
        //lex.advance();
    }
    output_file << "</tokens>";
}
int main(int argc, char* argv[]) {
    if(argc > 1) {
        lexer lex{argv[1]};
        run(lex);
        auto parser_output = lex.output_path().substr(0,lex.output_path().find("T.")) + ".xml";
        compilation_engine parser{lex.output_path(), parser_output};
        parser.compile_class();
    }
}