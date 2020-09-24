#ifndef JACK_TOKENIZER_H_
#define JACK_TOKENIZER_H_
#include"typedefs.h"
#include<string>
#include<map>
#include<set>
class lexer {
    public:
        explicit lexer(const std::string& file);
        bool has_more_tokens();
        tds::token_type token_type();
        std::pair<tds::keywords,std::string> key_word();
        void advance();
        char peek();
        char symbol();
        std::string identifier();
        tds::i16 int_val();
        std::string string_val();
        std::string output_path()  const {
            return out_path;
        }

        bool is_symbol(const char c);
    private:
        std::string source = "";
        std::string out_path = "";
        std::string cur_token = "";
        tds::u32 idx = 0;
        tds::u32 restart = 0;
        inline static const std::map<std::string, tds::keywords> keyword_map {{
            {"class",tds::keywords::class_}, {"constructor",tds::keywords::constructor},
            {"function",tds::keywords::function}, {"method",tds::keywords::method},
            {"field", tds::keywords::field}, {"static",tds::keywords::static_},
            {"var", tds::keywords::var}, {"int", tds::keywords::int_}, {"char", tds::keywords::char_},
            {"boolean",tds::keywords::boolean}, {"void", tds::keywords::void_},
            {"true", tds::keywords::true_}, {"false", tds::keywords::false_}, {"null", tds::keywords::null},
            {"this", tds::keywords::this_}, {"let", tds::keywords::let}, {"do", tds::keywords::do_},
            {"if", tds::keywords::if_}, {"else", tds::keywords::else_}, {"while", tds::keywords::while_},
            {"return", tds::keywords::return_}
        }};

        inline static const std::set<char> symbols {
            '{', '}', '(', ')', '[', ']', '.', ',', ';', '+',
            '-', '*', '/', '&', '|', '<','>', '=', '~'
        };
        void comment();
        void white_space();
};
#endif