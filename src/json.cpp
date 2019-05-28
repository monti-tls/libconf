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

#include "lconf/json.h"
#include <fstream>

namespace lconf { namespace json
{
    Node* parse(std::string const& file)
    {
        std::ifstream fs(file, std::ios::in);
        if (!fs)
            throw std::logic_error("json::parse: unable to open \"" + file + "\"");
        return parse(fs);
    }

    Node* parse(std::istream& file)
    {
        Lexer lexer(file);
        Parser parser(lexer);
        return parser.parse();
    }

    void serialize(Node* node, std::string const& file, bool indent)
    {
        std::ofstream fs(file, std::ios::out);
        if (!fs)
            throw std::logic_error("json::serialize: unable to open\"" + file + "\"");
        serialize(node, fs, indent);
    }

    void serialize(Node* node, std::ostream& file, bool indent)
    {
        node->serialize(file, indent);
    }

    void extract(Template const& tpl, std::string const& file)
    {
        Node* node = parse(file);
        tpl.extract(node);
        delete node;
    }

    void extract(Template const& tpl, std::istream& file)
    {
        Node* node = parse(file);
        tpl.extract(node);
        delete node;
    }

    void synthetize(Template const& tpl, std::string const& file, bool indent)
    {
        Node* node = tpl.synthetize();
        serialize(node, file, indent);
        delete node;
    }

    void synthetize(Template const& tpl, std::ostream& file, bool indent)
    {
        Node* node = tpl.synthetize();
        serialize(node, file, indent);
        delete node;
    }
} }
