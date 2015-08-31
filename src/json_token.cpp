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

#include "json_token.h"

using namespace json;

Token::Token(Token::Type type, std::string const& value) :
    m_type(type),
    m_value(value)
{
    m_info.empty = true;
}

Token::Type Token::type() const
{ return m_type; }

std::string const& Token::value() const
{ return m_value; }

void Token::setInfo(Token::Info const& info)
{
    m_info = info;
    m_info.empty = false;
}

Token::Info const& Token::info() const
{ return m_info; }
