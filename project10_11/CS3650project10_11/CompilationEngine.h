#ifndef COMPILATION_ENGINE_H_
#define COMPILATION_ENGINE_H_
#include"typedefs.h"
#include<string>
#include<vector>
#include<fstream>
#include<set>
class compilation_engine {
    public:
        compilation_engine(const std::string& input_file, const std::string& output_file);
        void compile_class();
        void compile_class_var_dec();
        void compile_subroutine();
        void compile_parameter_list();
        void compile_var_dec();
        void compile_statements();
        void compile_do();
        void compile_let();
        void compile_while();
        void compile_return();
        void compile_if();
        void compile_expression();
        void compile_term();
        void compile_expression_list();
        void compile_subroutine_body();
    private:
        std::ofstream output_file;
        std::string input_code = "";
        std::string output_path = "";
        bool syntax_error = false;
        tds::u32 comp_idx = 0;
        tds::u32 depth = 1;
        std::vector<tds::token> tokens;
        std::vector<tds::token> get_tokens();
        inline static const std::set<std::string> primatives {"int", "boolean", "char", "Array", "Keyboard"};
        inline static const std::set<std::string> operators {"+", "-", "/", "*", "&", "|","&lt;","&gt;","&eq;","-","~"};
        void compile_var_list();
        void write_to_file();
        void identifier();
        void type();
        void match(const std::string& c);
        void consume_op();
        bool is_operator();
        bool is_type();
        void advance();

};
#endif