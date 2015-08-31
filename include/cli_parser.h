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
#include <iostream>

namespace cli
{
    class Parser
    {
    public:
        Parser(int argc, char** argv);
        ~Parser();
        
        void setProgramDescription(std::string const& description);
        std::string const& programDescription() const;
        
        Option& addSwitch(char shortName, std::string const& longName = "");
        Option& addOption(char shortName, std::string const& longName = "");
        
        Option* find(char shortName) const;
        Option* find(std::string const& longName) const;
        
        bool exists(char shortName) const;
        bool exists(std::string const& longName) const;
        
        void parse();
        
        bool has(char shortName) const;
        bool has(std::string const& longName) const;
        bool has(Option* opt) const;
        
        std::string value(char shortName) const;
        std::string value(std::string const& longName) const;
        std::string value(Option* opt) const;
        
        void showHelp(std::ostream& out = std::cout) const;
        
    private:
        std::string m_programName;
        std::string m_programDescription;
        std::istringstream m_in;
        std::set<Option*> m_options;
        std::map<Option*, std::string> m_values;
    };
}

#endif // LCONF_CLI_PARSER_H
