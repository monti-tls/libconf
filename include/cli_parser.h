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

#ifndef LCONF_CLI_PARSER_H
#define LCONF_CLI_PARSER_H

#include "cli_option.h"
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <iostream>

namespace cli
{
    //! A CLI option string parser.
    //! It works with a given set of switches (non-valued options)
    //!   and valued options (simply referred to as options).
    //! One can set the program description (or not), and generate an
    //!   automatic help message using showHelp().
    //! The command line option string can have the following form :
    //!   commandName [options] [arguments]
    //! Where arguments are whitespace-separated.
    class Parser
    {
    public:
        //! Create a CLI parser from main() arguments.
        Parser(int argc, char** argv);
        ~Parser();
        
        //! Set the program description for the help message (optional).
        void setProgramDescription(std::string const& description);
        //! Get the program description.
        std::string const& programDescription() const;
        
        //! Add a switch (e.g. non-valued) option to this parser.
        Option& addSwitch(char shortName, std::string const& longName = "");
        //! Add a valued option to this parser.
        Option& addOption(char shortName, std::string const& longName = "");
        
        //! Find an option by short name.
        Option* find(char shortName) const;
        //! Find an option by long name.
        Option* find(std::string const& longName) const;
        
        //! Check whether an option exists by its short name.
        bool exists(char shortName) const;
        //! Check whether an option exists by its long name.
        bool exists(std::string const& longName) const;
        
        //! Parse the given CLI options.
        void parse();
        
        //! Get the program name, as invoked from the command line.
        std::string const& programName() const;
        
        //! Check if an option was given in the CLI string by short name.
        bool has(char shortName) const;
        //! Check if an option was given in the CLI string by long name.
        bool has(std::string const& longName) const;
        //! Check if an option was given in the CLI string by value.
        bool has(Option* opt) const;
        
        //! Get the option value by short name.
        std::string value(char shortName) const;
        //! Get the option value by long name.
        std::string value(std::string const& longName) const;
        //! Get the option value by value name.
        std::string value(Option* opt) const;
        
        //! Get the CLI arguments vector.
        std::vector<std::string> const& arguments() const;
        
        //! Generate the help message to the given stream.
        void showHelp(std::ostream& out = std::cout) const;
        
    private:
        void M_skip();
        int M_get();
        
    private:
        std::string m_programName;
        std::string m_programDescription;
        
        std::istringstream m_in;
        int m_nextChar;
        
        std::set<Option*> m_options;
        std::map<Option*, std::string> m_values;
        std::vector<std::string> m_arguments;
    };
}

#endif // LCONF_CLI_PARSER_H
