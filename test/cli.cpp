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
    
    try
    {
        // Here is a sample CLI arguments & options parser example.
        // The parser is created from main's arguments.
        Parser parser(argc, argv);
        // You add options/switches this way, each modifier (Option::set*) returns a reference,
        //   so calls can be chained.
        // Descriptions are optional (the help message is generated accordingly).
        Option& a = parser.addSwitch('h', "help").setDescription("Display this help").setStop();
        Option& b = parser.addSwitch('b').setDescription("Switch -b");
        Option& c = parser.addOption('c', "c-option").setRequired();
        Option& d = parser.addSwitch('d', "d-switch");
        // Here we set up a simple program description.
        parser.setProgramDescription("My funny program !");
        
        // Parse the option string.
        parser.parse();

        // Options can be retrieved by value (w/ the address of the return value from
        //   add{Option, Swtich}.
        if (parser.has(&a))
        {
            parser.showHelp();
            return 0;
        }
        
        // One can also address options by short name or by long name, w/
        //   value, has, find.
        std::cout << "program name: " << parser.programName() << std::endl;
        std::cout << "-b switch: " << (parser.has(&b) ? "yes" : "no") << std::endl;
        std::cout << "-c option: " << (parser.has(&c) ? ("yes - value = " + parser.value("c-option")) : "no") << std::endl;
        std::cout << "-d switch: " << (parser.has(&d) ? "yes" : "no") << std::endl;
        
        // Arguments (if any) are splitted into a std::vector.
        std::cout << "Given arguments :" << std::endl;
        for (unsigned int i = 0; i < parser.arguments().size(); ++i)
            std::cout << "arg #" << i << " = " << parser.arguments()[i] << std::endl;
    }
    catch(std::exception const& exc)
    {
        std::cerr << "Exception:\n\t" << exc.what() << std::endl;
    }

    return 0;
}
