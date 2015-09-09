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

#ifndef LCONF_JSON_TOKEN_H
#define LCONF_JSON_TOKEN_H

#include <string>

namespace lconf { namespace json
{
    class Token
    {
    public:
        enum Type
        {
            Eof,
            Bad,
            LeftBrace,
            RightBrace,
            LeftBracket,
            RightBracket,
            Comma,
            Colon,
            True,
            False,
            Number,
            String
        };
        
        struct Info
        {
            bool empty;
            int line, column;
        };
        
    public:
        Token(Type type = Bad, std::string const& value = "");
        
        Type type() const;
        std::string const& value() const;
        
        void setInfo(Info const& info);
        Info const& info() const;
        
    private:
        Type m_type;
        std::string m_value;
        Info m_info;
    };
} }

#endif // LCONF_JSON_TOKEN_H
