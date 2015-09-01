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
    //! A class representing a command-line (CLI) option.
    //! An option can have a short name (e.g. -c) and/or a long
    //!   name (e.g. --long-name).
    //! The description is used in the auto-generated help.
    //! Options can take a value (e.g., are valued), and
    //!   can be required to start the program.
    //! The setStop() modifier is used to stop the parser when
    //!   this option is encountered (useful for help option for example).
    class Option
    {
    public:
        //! Create an option with a given short name, long name and properties.
        Option(char shortName, std::string const& longName, bool valued);
        ~Option();
        
        //! Set the help description for this option.
        Option& setDescription(std::string const& description);
        //! Set this option required.
        Option& setRequired(bool required = true);
        //! Set this option to stop the parser.
        Option& setStop(bool stop = true);
        
        //! Get the short name of this option.
        char shortName() const;
        //! Get the long name of this option.
        std::string const& longName() const;
        //! Returns whether or not this option takes a value.
        bool valued() const;
        //! Returns whether or not this option is mandatory.
        bool required() const;
        //! Returns whether or not this option stops the parser.
        bool stop() const;
        //! Returns the help description of this option.
        std::string const& description() const;
        
    private:
        char m_shortName;
        std::string m_longName;
        bool m_valued;
        bool m_required;
        bool m_stop;
        std::string m_description;
    };
}

#endif // LCONF_CLI_OPTION_H
