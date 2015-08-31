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

#ifndef LCONF_CLI_OPTION_H
#define LCONF_CLI_OPTION_H

#include <string>

namespace cli
{
    class Option
    {
    public:
        Option(char shortName, std::string const& longName, bool valued);
        ~Option();
        
        void setDescription(std::string const& description);
        
        char shortName() const;
        std::string const& longName() const;
        bool valued() const;
        std::string const& description() const;
        
    private:
        char m_shortName;
        std::string m_longName;
        bool m_valued;
        std::string m_description;
    };
}

#endif // LCONF_CLI_OPTION_H
