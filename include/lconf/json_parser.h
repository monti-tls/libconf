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

#ifndef LCONF_JSON_PARSER_H
#define LCONF_JSON_PARSER_H

#include "lconf/json_lexer.h"
#include "lconf/json_node.h"

namespace lconf { namespace json
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
} }

#endif // LCONF_JSON_PARSER_H
