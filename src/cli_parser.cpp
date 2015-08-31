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

#include "cli_parser.h"

using namespace cli;

Parser::Parser(int argc, char** argv) :
    m_programDescription("")
{
    m_programName = argv[0];
    
    std::ostringstream ss;
    for (int i = 1; i < argc; ++i)
        ss << argv[i];
}

Parser::~Parser()
{
    std::set<Option*>::const_iterator it = m_options.begin();
    for (; it != m_options.end(); ++it)
        delete *it;
}

void Parser::setProgramDescription(std::string const& description)
{
    m_programDescription = description;
}

std::string const& Parser::programDescription() const
{
    return m_programDescription;
}

Option& Parser::addSwitch(char shortName, std::string const& longName)
{
    Option* option = new Option(shortName, longName, false);
    m_options.insert(option);
    return *option;
}

Option& Parser::addOption(char shortName, std::string const& longName)
{
    Option* option = new Option(shortName, longName, true);
    m_options.insert(option);
    return *option;
}

Option* Parser::find(char shortName) const
{
    std::set<Option*>::iterator it = m_options.begin();
    for (; it != m_options.end(); ++it)
    {
        if ((*it)->shortName() == shortName)
            return *it;
    }
    
    return 0;
}

Option* Parser::find(std::string const& longName) const
{
    if (!longName.size())
        return 0;
    
    std::set<Option*>::const_iterator it = m_options.begin();
    for (; it != m_options.end(); ++it)
    {
        if ((*it)->longName() == longName)
            return *it;
    }
    
    return 0;
}

bool Parser::exists(char shortName) const
{
    return find(shortName);
}

bool Parser::exists(std::string const& longName) const
{
    return find(longName);
}

void Parser::parse()
{
    m_values.clear();
}

bool Parser::has(char shortName) const
{
    return has(find(shortName));
}

bool Parser::has(std::string const& longName) const
{
    return has(find(longName));
}

bool Parser::has(Option* opt) const
{
    if (!opt)
        return false;
    
    std::map<Option*, std::string>::const_iterator it = m_values.find(opt);
    return it != m_values.end();
}

std::string Parser::value(char shortName) const
{
    return value(find(shortName));
}

std::string Parser::value(std::string const& longName) const
{
    return value(find(longName));
}

std::string Parser::value(Option* opt) const
{
    std::map<Option*, std::string>::const_iterator it = m_values.find(opt);
    if (it != m_values.end())
        return it->second;
    
    return "";
}

void Parser::showHelp(std::ostream& out) const
{
    out << "usage: " << m_programName << " [options]" << std::endl;
    
    if (m_programDescription.size())
        out << std::endl << m_programDescription << std::endl << std::endl;
    
    out << "available options:" << std::endl;
    
    std::set<Option*>::const_iterator it;
    
    int width = 0;
    for (it = m_options.begin(); it != m_options.end(); ++it)
    {
        int w = (*it)->longName().size();
        if (w > width)
            width = w;
    }
    
    for (it = m_options.begin(); it != m_options.end(); ++it)
    {
        Option* option = *it;
        
        int w = option->longName().size();
        out << "  -" << option->shortName();
        
        if (option->longName().size())
            out << ", --" << option->longName();
        else
            out << "    ";
        
        if (option->valued())
            out << " <value>";
        else
            out << "        ";
        
        for (int j = 0; j < width-w; ++j)
            out << " ";
        
        if (option->description().size())
            out << ": " << option->description() << ".";
        
        out << std::endl;
    }
}