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

#include "cli_option.h"
#include "cli_parser.h"

int main(int argc, char** argv)
{
    using namespace cli;
    
    Parser parser(argc, argv);
    Option& a = parser.addOption('a', "aaaa").setDescription("example description");
    Option& b = parser.addSwitch('b').setDescription("another description");
    Option& c = parser.addOption('c', "cc").setRequired();
    Option& d = parser.addSwitch('d', "dd");
    parser.setProgramDescription("My funny program !");
    
    // parser.showHelp();
    
    parser.parse();
    
    if (parser.has(&a))
        std::cout << "a = " << parser.value(&a) << std::endl;
    if (parser.has(&b))
        std::cout << "b = " << parser.value(&b) << std::endl;
    if (parser.has(&c))
        std::cout << "c = " << parser.value('c') << std::endl;
    if (parser.has(&d))
        std::cout << "d = " << parser.value("dd") << std::endl;
    
    for (unsigned int i = 0; i < parser.arguments().size(); ++i)
        std::cout << "arg #" << i << " = " << parser.arguments()[i] << std::endl;

    return 0;
}
