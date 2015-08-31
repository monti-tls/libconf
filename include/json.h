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

#ifndef LCONF_JSON_H
#define LCONF_JSON_H

#include "json_token.h"
#include "json_lexer.h"
#include "json_node.h"
#include "json_parser.h"
#include "json_template.h"
#include <string>
#include <iostream>

namespace json
{
    Node* parse(std::string const& file);
    Node* parse(std::istream& file);
    void extract(Template const& tpl, std::string const& file);
    void extract(Template const& tpl, std::istream& file);
}

#endif // LCONF_JSON_H
