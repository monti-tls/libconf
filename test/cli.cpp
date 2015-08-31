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
    parser.addOption('a', "aaaa").setDescription("example description");
    parser.addSwitch('b', "").setDescription("another description");
    parser.addOption('c', "cc");
    parser.addSwitch('d', "dd");
    parser.setProgramDescription("My funny program !");
    
    parser.showHelp();

    return 0;
}
