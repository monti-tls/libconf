/* This file is part of libconf.
 * 
 * libconf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libconf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with libconf.  If not, see <http://www.gnu.org/licenses/>.
 */

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
