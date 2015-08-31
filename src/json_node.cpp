#include "json_node.h"

using namespace json;

// Numeric value node

NumberNode::NumberNode(float value) :
    m_value(value)
{}

Node::Type NumberNode::type() const
{ return Number; }

float NumberNode::value() const
{ return m_value; }

// Boolean value node

BooleanNode::BooleanNode(bool value) :
    m_value(value)
{}

Node::Type BooleanNode::type() const
{ return Boolean; }

bool BooleanNode::value() const
{ return m_value; }

// String value node

StringNode::StringNode(std::string const& value) :
    m_value(value)
{}

Node::Type StringNode::type() const
{ return String; }

std::string const& StringNode::value() const
{ return m_value; }

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
    if (i >= m_impl.size()) throw std::domain_error("ArrayNode::at: index out of bounds");
    return m_impl[i];
}

Node* ArrayNode::at(size_t i) const
{
    if (i >= m_impl.size()) throw std::domain_error("ArrayNode::at: index out of bounds");
    return m_impl[i];
}

std::vector<Node*>& ArrayNode::impl()
{ return m_impl; }

std::vector<Node*> const& ArrayNode::impl() const
{ return m_impl; }
