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

#include "lconf/json_node.h"
#include <iomanip>

using namespace lconf;
using namespace json;

// Abstract node class
std::string Node::typeName(Node::Type type)
{
    if (type == Number) return "Number";
    else if (type == Boolean) return "Boolean";
    else if (type == String) return "String";
    else if (type == Object) return "Object";
    else if (type == Array) return "Array";
    
    return "?";
}

void Node::serialize(std::ostream& out, bool indent) const
{
    M_serialize(out, 0, indent);
}

// Numeric value node

NumberNode::NumberNode(float value) :
    m_value(value)
{}

Node::Type NumberNode::type() const
{ return Number; }

float NumberNode::value() const
{ return m_value; }

void NumberNode::M_serialize(std::ostream& out, int level, bool indent) const
{
    std::string pre = "";
    for (int i = 0; indent && i < level; ++i) pre += " ";
    
    out << pre << m_value;
}

bool NumberNode::M_multiline() const
{
    return false;
}

// Boolean value node

BooleanNode::BooleanNode(bool value) :
    m_value(value)
{}

Node::Type BooleanNode::type() const
{ return Boolean; }

bool BooleanNode::value() const
{ return m_value; }

void BooleanNode::M_serialize(std::ostream& out, int level, bool indent) const
{
    std::string pre = "";
    for (int i = 0; indent && i < level; ++i) pre += " ";
    
    out << pre << (m_value ? "true" : "false");
}

bool BooleanNode::M_multiline() const
{
    return false;
}

// String value node

StringNode::StringNode(std::string const& value) :
    m_value(value)
{}

Node::Type StringNode::type() const
{ return String; }

std::string const& StringNode::value() const
{ return m_value; }

std::string StringNode::escapedValue() const
{
    std::string escaped;

    for (auto c : m_value)
    {
        if (c == '\n')
            escaped += "\\n";
        else if (c == '\t')
            escaped += "\\t";
        else if (c == '"')
            escaped += "\\\"";
        else
            escaped += c;
    }

    return std::move(escaped);
}

void StringNode::M_serialize(std::ostream& out, int level, bool indent) const
{
    std::string pre = "";
    for (int i = 0; indent && i < level; ++i) pre += " ";
    
    out << pre << '"' << escapedValue() << '"';
}

bool StringNode::M_multiline() const
{
    return false;
}

// Object node

ObjectNode::ObjectNode()
{}

ObjectNode::~ObjectNode()
{
    for (std::map<std::string, Node*>::iterator it = m_impl.begin();
         it != m_impl.end(); ++it)
         delete it->second;
}

Node::Type ObjectNode::type() const
{ return Object; }

bool ObjectNode::exists(std::string const& key)
{ return m_impl.find(key) != m_impl.end(); }

Node*& ObjectNode::get(std::string const& key)
{ return m_impl[key]; }

Node* ObjectNode::get(std::string const& key) const
{ return m_impl.at(key); }

std::map<std::string, Node*>& ObjectNode::impl()
{ return m_impl; }

std::map<std::string, Node*> const& ObjectNode::impl() const
{ return m_impl; }

void ObjectNode::M_serialize(std::ostream& out, int level, bool indent) const
{
    std::string pre = "";
    for (int i = 0; indent && i < level; ++i) pre += " ";
    
    out << pre << '{';
    if (indent) out << std::endl;
    
    std::map<std::string, Node*>::const_iterator it;
    for (it = m_impl.begin(); it != m_impl.end(); ++it)
    {
        if (indent) out << pre << "    ";
        out << '"' << it->first << "\": ";
        
        if (indent && it->second->M_multiline())
        {
            out << std::endl;
            it->second->M_serialize(out, level + 4, indent);
        }
        else
        {
            it->second->M_serialize(out, 0, false);
        }
        
        // (++it)-- returns the next iterator value, leaving
        //   it unchanged.
        if ((++it)-- != m_impl.end())
            out << ", ";
        if (indent) out << std::endl;
    }
    
    out << pre << '}';
}

bool ObjectNode::M_multiline() const
{
    return true;
}

// Array node

ArrayNode::ArrayNode()
{}

ArrayNode::~ArrayNode()
{
    for (unsigned int i = 0; i < m_impl.size(); ++i)
        delete m_impl[i];
}

Node::Type ArrayNode::type() const
{ return Array; }

size_t ArrayNode::size() const
{ return m_impl.size(); }

Node*& ArrayNode::at(size_t i)
{
    if (i >= m_impl.size()) throw std::domain_error("json::ArrayNode::at: index out of bounds");
    return m_impl[i];
}

Node* ArrayNode::at(size_t i) const
{
    if (i >= m_impl.size()) throw std::domain_error("json::ArrayNode::at: index out of bounds");
    return m_impl[i];
}

std::vector<Node*>& ArrayNode::impl()
{ return m_impl; }

std::vector<Node*> const& ArrayNode::impl() const
{ return m_impl; }

void ArrayNode::M_serialize(std::ostream& out, int level, bool indent) const
{
    std::string pre = "";
    for (int i = 0; indent && i < level; ++i) pre += " ";
    
    out << pre << '[';
    bool multi = indent && M_multiline();
    if (multi) out << std::endl;
    
    for (unsigned int i = 0; i < m_impl.size(); ++i)
    {
        if (multi)
        {
            m_impl[i]->M_serialize(out, level + 4, indent);
        }
        else
        {
            m_impl[i]->M_serialize(out, 0, false);
        }
        
        if (i != m_impl.size()-1)
            out << ", ";
        if (multi) out << std::endl;
    }
    
    out << pre << ']';
}

bool ArrayNode::M_multiline() const
{
    for (unsigned int i = 0; i < m_impl.size(); ++i)
        if (m_impl[i]->M_multiline())
            return true;
    return false;
}
