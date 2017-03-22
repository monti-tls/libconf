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

#include "lconf/json.h"
#include "lconf/json_template.h"

#include <sstream>

int main()
{
    using namespace lconf;
    using namespace json;

    try
    {
        // Set up the JSON input stream (one can use any std::istream).
        std::string val =
        "{\n"
        "   \"a\" : 123,\n"
        "   \"c\" : @\"test/inc.json\"\n"
        "}"
        ;
        std::istringstream ss;
        ss.str(val);

        Node* node = parse(ss);

        // Serialize the template, now the values must have changed
        std::cout << "Serialized  :" << std::endl;
        json::serialize(node, std::cout);
        std::cout << std::endl;

        delete node;
    }
    catch(Exception const& exc)
    {
        // Here you can retrieve the offending node :
        Node* offending = exc.node();
        std::cerr << "Exception:[" << offending << "]\n\t" << exc.what() << std::endl;
    }
    catch(std::exception const& exc)
    {
        std::cerr << "Exception:\n\t" << exc.what() << std::endl;
    }

    return 0;
}
