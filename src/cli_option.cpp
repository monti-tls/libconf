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

using namespace cli;

Option::Option(char shortName, std::string const& longName, bool valued) :
    m_shortName(shortName),
    m_longName(longName),
    m_valued(valued),
    m_required(false),
    m_stop(false),
    m_description("")
{}

Option::~Option()
{}

Option& Option::setDescription(std::string const& description)
{
    m_description = description;
    return *this;
}

Option& Option::setRequired(bool required)
{
    m_required = required;
    return *this;
}

Option& Option::setStop(bool stop)
{
    m_stop = stop;
    return *this;
}

char Option::shortName() const
{ return m_shortName; }

std::string const& Option::longName() const
{ return m_longName; }

bool Option::valued() const
{ return m_valued; }

bool Option::required() const
{ return m_required; }

bool Option::stop() const
{ return m_stop; }

std::string const& Option::description() const
{ return m_description; }
