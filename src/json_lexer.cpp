/* This file is part of libconf.
 * 
 * Copyright (c) 2015, Alexandre Monti
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

#include "lconf/json_lexer.h"
#include <fstream>

using namespace lconf;
using namespace json;

Lexer::Lexer(std::istream& in) :
    m_in(in)
{
    M_init();
}

Lexer::~Lexer()
{
}

Token Lexer::get()
{
    Token tok = m_nextToken;
    m_nextToken = M_getToken();
    return tok;
}

Token const& Lexer::seek() const
{ return m_nextToken; }

//! Init the lexer (called from constructors).
void Lexer::M_init()
{
    m_currentInfo.line = 1;
    m_currentInfo.column = 0;
    
    // Get first char (m_nextChar is now valid)
    m_nextChar = 0;
    M_getChar();
    // Get first token (m_nextToken is now valid)
    m_nextToken = M_getToken();
}

//! Exctract a character from the input stream.
//! This method manages the position in the input
//!   stream for debug and error messages purposes.
int Lexer::M_getChar()
{
    int ch = m_nextChar;
    m_nextChar = m_in.get();
    
    // Update stream information
    if (ch == '\n')
    {
        ++m_currentInfo.line;
        m_currentInfo.column = 0;
    }
    ++m_currentInfo.column;
    
    return ch;
}

//! Skip whitespaces (and new lines).
void Lexer::M_skipWs()
{
    while (std::isspace(m_nextChar) || m_nextChar == '\n')
    {
        M_getChar();
        if (m_nextChar < 0) return;
    }
}

//! Skip comments, starting with a hashtag '#'.
void Lexer::M_skipComments()
{
    while (m_nextChar == '#')
    {
        while (m_nextChar != '\n')
        {
            M_getChar();
            if (m_nextChar < 0) return;
        }
        
        M_skipWs();
    }
}

//! Skip whitespaces and comments.
void Lexer::M_skip()
{
    M_skipWs();
    M_skipComments();
    M_skipWs();
}

//! Extract a token from the input stream.
Token Lexer::M_getToken()
{
    // Skip whitespaces and comments
    M_skip();
    
    // Create token (bad by default), and save current stream information
    Token token = Token::Bad;
    Token::Info info = m_currentInfo;
    
    // Handle EOF gracefully
    if (m_nextChar < 0)
        token = Token::Eof;
    else
    {
        bool eatlast = true;
        
        if (m_nextChar == '{')
            token = Token::LeftBrace;
        else if (m_nextChar == '}')
            token = Token::RightBrace;
        else if (m_nextChar == '[')
            token = Token::LeftBracket;
        else if (m_nextChar == ']')
            token = Token::RightBracket;
        else if (m_nextChar == ',')
            token = Token::Comma;
        else if (m_nextChar == ':')
            token = Token::Colon;
        else if (m_nextChar == 't')
            token = M_matchKeyword(Token::True, "true");
        else if (m_nextChar == 'f')
            token = M_matchKeyword(Token::False, "false");
        else
        {
            // String and identifiers
            if (m_nextChar == '"')
            {
                // Eat the double quotes
                M_getChar();
                
                std::string value;
                bool ok = true;
                
                while (ok && m_nextChar != '"')
                {
                    // Stop if EOF is encountered
                    if (m_nextChar < 0)
                        ok = false;
                    
                    // Handle some escape sequences
                    if (m_nextChar == '\\')
                    {
                        // Eat the backslash
                        M_getChar();
                        
                        // Get the escaped character (and handle EOF)
                        int ch = M_getChar();
                        if (ch < 0)
                            ok = false;
                        
                        if (ch == '\\')
                            value += '\\';
                        else if (ch == '"')
                            value += '"';
                        else if (ch == 'n')
                            value += '\n';
                        else if (ch == 't')
                            value += 't';
                        else
                            ok = false;
                    }
                    // Simple character
                    else
                        value += M_getChar();
                }
                
                // Strings must end with another double quotes
                ok = ok && m_nextChar == '"';
                
                if (!ok)
                    token = Token::Bad;
                else
                    token = Token(Token::String, value);
            }
            // Numbers
            if (m_nextChar == '-' || m_nextChar == '.' || std::isdigit(m_nextChar))
            {
                std::string value;
                bool ok = true;
                
                // Eventual sign
                if (m_nextChar == '-')
                {
                    value += M_getChar();
                    if (m_nextChar < 0)
                        ok = false;
                }
                
                // Eventual integer part
                while (ok && std::isdigit(m_nextChar))
                {
                    value += M_getChar();
                    if (m_nextChar < 0)
                        ok = false;
                }
                
                // Eventual floating part
                if (ok && m_nextChar == '.')
                {
                    // Eat the dot
                    value += M_getChar();
                    if (m_nextChar < 0)
                        ok = false;
                    
                    // Don't allow empty floating parts
                    //   (as we already allow empty integer parts, we
                    //   would end up with '.' as a valid number...)
                    if (!std::isdigit(m_nextChar))
                        ok = false;
                    
                    // Get the floating part
                    while (ok && std::isdigit(m_nextChar))
                    {
                        value += M_getChar();
                        if (m_nextChar < 0)
                            ok = false;
                    }
                }

                // Eventual exponent part
                if (ok && (m_nextChar == 'e' || m_nextChar == 'E'))
                {
                    // Eat the 'e'
                    value += M_getChar();
                    if (m_nextChar < 0)
                        ok = false;

                    // Eventual exponent's sign
                    if (m_nextChar == '-')
                    {
                        value += M_getChar();
                        if (m_nextChar < 0)
                            ok = false;
                    }

                    if (!std::isdigit(m_nextChar))
                        ok = false;

                    while (ok && std::isdigit(m_nextChar))
                    {
                        value += M_getChar();
                        if (m_nextChar < 0)
                            ok = false;
                    }
                }
                
                if (!ok)
                    token = Token::Bad;
                else
                    token = Token(Token::Number, value);
                
                eatlast = false;
            }
        }
        
        // Get the last char from previous rules
        if (eatlast && token.type() != Token::Bad)
            M_getChar();
    }
    
    // Set stream information for this token and return
    token.setInfo(info);
    return token;
}

//! Match a keyword in the input stream, returning a token
//!   with the given type (or a Bad one in case of a mismatch).
Token Lexer::M_matchKeyword(Token::Type type, std::string const& kw)
{
    for (unsigned int i = 0; i < kw.size(); ++i)
    {
        if (m_nextChar != kw[i])
            return Token::Bad;
        if (i == kw.size()-1)
            break;
        M_getChar();
    }
    
    return type;
}
