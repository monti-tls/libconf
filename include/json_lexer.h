#ifndef LCONF_JSON_LEXER_H
#define LCONF_JSON_LEXER_H

#include "json_token.h"
#include <iostream>

namespace json
{
    class Lexer
    {
    public:
        Lexer(std::istream& in);
        ~Lexer();
        
        //! Get the next token from the input stream.
        Token get();
        //! Seek for the next token in the input stream
        //!   (but do NOT extract it).
        Token const& seek() const;
        
    private:
        void M_init();
        int M_getChar();
        void M_skipWs();
        void M_skipComments();
        void M_skip();
        Token M_getToken();
        
        Token M_matchKeyword(Token::Type type, std::string const& kw);
        
    private:
        std::istream& m_in;
        
        int m_nextChar;
        Token m_nextToken;
        Token::Info m_currentInfo;
    };
}

#endif // LCONF_JSON_LEXER_H
