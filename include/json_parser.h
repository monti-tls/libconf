#ifndef LCONF_JSON_PARSER_H
#define LCONF_JSON_PARSER_H

#include "json_lexer.h"
#include "json_node.h"

namespace json
{
    class Parser
    {
    public:
        Parser(Lexer& lex);
        ~Parser();
        
        Node* parse();
        
    private:
        Node* M_atom();
        Node* M_object();
        Node* M_array();
        
        void M_error(Token const& at, std::string const& msg);
        
    private:
        Lexer& m_lex;
    };
}

#endif // LCONF_JSON_PARSER_H
