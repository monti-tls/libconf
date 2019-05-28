/* This file is part of libconf.
 *
 * Copyright (c) 2015 - 2019, Alexandre Monti
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

#include "lconf/json_parser.h"
#include "lconf/json.h"
#include <stdexcept>
#include <sstream>

using namespace lconf;
using namespace json;

Parser::Parser(Lexer& lex) :
    m_lex(lex)
{}

Parser::~Parser()
{}

Node* Parser::parse()
{
    if (m_lex.seek().type() == Token::LeftBrace)
        return M_object();

    return M_array();
}

Node* Parser::M_atom()
{
    Token next = m_lex.seek();
    if (next.type() == Token::Bad)
    {
        M_error(next, "bad token");
    }
    else if (next.type() == Token::True ||
             next.type() == Token::False)
    {
        m_lex.get();
        return new BooleanNode(next.type() == Token::True);
    }
    else if (next.type() == Token::Number)
    {
        m_lex.get();
        
        float value;
        try {
            value = std::stof(next.value());
        } catch (std::exception const&) {
            value = 0.0f;
        }

        return new NumberNode(value);
    }
    else if (next.type() == Token::String)
    {
        m_lex.get();
        return new StringNode(next.value());
    }
    else if (next.type() == Token::LeftBrace)
        return M_object();
    else if (next.type() == Token::LeftBracket)
        return M_array();
    else if (next.type() == Token::Include)
    {
        Node* tree = ::parse(next.value());
        m_lex.get();
        return tree;
    }

    return 0;
}

Node* Parser::M_object()
{
    // Eat the opening {
    if (m_lex.seek().type() != Token::LeftBrace)
        M_error(m_lex.seek(), "expected `{' at beginning of object declaration");
    m_lex.get();

    // Create appropriate node
    ObjectNode* node = new ObjectNode();

    // Parse object entries
    for (;;)
    {
        // Allow empty objects
        if (m_lex.seek().type() == Token::RightBrace)
            break;

        // Get key identifier
        if (m_lex.seek().type() != Token::String)
            M_error(m_lex.seek(), "expected a identifier key");
        Token token = m_lex.get();
        std::string key = token.value();

        if (node->exists(key))
            M_error(token, "redifinition of object entry `" + key + "'");

        // Get the separator
        if (m_lex.seek().type() != Token::Colon)
            M_error(m_lex.seek(), "expected `:' after identifier");
        m_lex.get();

        // Parse the object element value
        node->impl()[key] = M_atom();

        // Eat comma, if needed
        if (m_lex.seek().type() == Token::Comma)
            m_lex.get();
        else
            break;
    }

    // Eat the closing }
    if (m_lex.seek().type() != Token::RightBrace)
        M_error(m_lex.seek(), "expected `}' at end of object declaration");
    m_lex.get();

    return node;
}

Node* Parser::M_array()
{
    // Eat the opening [
    if (m_lex.seek().type() != Token::LeftBracket)
        M_error(m_lex.seek(), "expected `[' at beginning of array definition");
    m_lex.get();

    // Create appropriate node
    ArrayNode* node = new ArrayNode();

    // Parse array entries
    for (;;)
    {
        // Allow empty arrays
        if (m_lex.seek().type() == Token::RightBracket)
            break;

        // Get array element
        node->impl().push_back(M_atom());

        // Get comma, if needed
        if (m_lex.seek().type() == Token::Comma)
            m_lex.get();
        else
            break;
    };

    if (m_lex.seek().type() != Token::RightBracket)
        M_error(m_lex.seek(), "expected `]' at end of array declaration");
    m_lex.get();

    return node;
}

void Parser::M_error(Token const& at, std::string const& msg)
{
    std::ostringstream ss;
    ss << "json::Parser::M_error: [" << at.info().line
       << ":" << at.info().column << "]"
       << ": " << msg;

    throw std::logic_error(ss.str());
}
