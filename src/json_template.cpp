#include "json_template.h"
#include <stdexcept>

using namespace json;

Exception::Exception(Node* node, std::string const& what) :
    std::logic_error(what),
    m_node(node)
{}

Node* Exception::node() const
{ return m_node; }

Element::Element() :
    refs(1)
{}

Element::~Element()
{}

Object::Object()
{}

Object::~Object()
{
    for (std::map<std::string, Element*>::const_iterator it = m_elements.begin();
         it != m_elements.end(); ++it)
    {
        if (!--it->second->refs)
            delete it->second;
    }
}

void Object::bind(std::string const& name, Element* elem)
{
    if (m_elements.find(name) != m_elements.end())
        throw std::logic_error("json::Object: element `" + name + "' is already bound");
    
    ++elem->refs;
    m_elements[name] = elem;
}

Element::Type Object::type() const
{ return Element::Object; }

void Object::extract(Node* node) const
{
    if (node->type() != Node::Object)
        throw Exception(node, "json::Object: type mismatch");
    ObjectNode* obj = node->downcast<ObjectNode>();
    
    for (std::map<std::string, Element*>::const_iterator it = m_elements.begin();
         it != m_elements.end(); ++it)
    {
        if (!obj->exists(it->first))
            throw Exception(node, "json::Object: missing element `" + it->first + "'");
        
        it->second->extract(obj->get(it->first));
    }
}

Node* Object::synthetize() const
{
    ObjectNode* obj = new ObjectNode();
    for (std::map<std::string, Element*>::const_iterator it = m_elements.begin();
         it != m_elements.end(); ++it)
    {
        obj->impl()[it->first] = it->second->synthetize();
    }
    return obj;
}

Array::Array()
{}

Array::~Array()
{
    for (unsigned int i = 0; i < m_elements.size(); ++i)
    {
        if (!--m_elements[i]->refs)
            delete m_elements[i];
    }
}

void Array::bind(Element* elem)
{
    ++elem->refs;
    m_elements.push_back(elem);
}

Element::Type Array::type() const
{ return Element::Array; }
    
void Array::extract(Node* node) const
{
    if (node->type() != Node::Array)
        throw Exception(node, "json::Array: type mismatch");
    ArrayNode* arr = node->downcast<ArrayNode>();
    
    for (unsigned int i = 0; i < m_elements.size(); ++i)
    {
        if (i >= arr->size())
            throw Exception(node, "json::Array: size mismatch in array");
        
        m_elements[i]->extract(arr->at(i));
    }
}

Node* Array::synthetize() const
{
    ArrayNode* arr = new ArrayNode();
    for (unsigned int i = 0; i < m_elements.size(); ++i)
    {
        arr->impl().push_back(m_elements[i]->synthetize());
    }
    return arr;
}

Template::Template() :
    m_impl(0)
{}

Template::Template(Template const& cpy) :
    m_impl(0)
{ operator=(cpy); }

Template::Template(Template& cpy) :
    m_impl(0)
{ operator=(cpy); }

Template::~Template()
{
    if (m_impl && !--m_impl->refs)
        delete m_impl;
}

Template& Template::operator=(Template const& cpy)
{
    m_impl = cpy.m_impl;
    if (m_impl)
        m_impl->refs++;
    return *this;
}

Template& Template::bind(std::string const& name, Template const& tpl)
{
    if (m_impl && m_impl->type() != Element::Object)
        throw std::logic_error("json::Template: template is already bound");
    
    if (!m_impl)
        m_impl = new Object();
    
    ((Object*) m_impl)->bind(name, tpl.m_impl);
    return *this;
}

Template& Template::bind_array(Template const& tpl)
{
    if (m_impl && m_impl->type() != Element::Array)
        throw std::logic_error("json::Template: template is already bound");
    
    if (!m_impl)
        m_impl = new Array();
    
    ((Array*) m_impl)->bind(tpl.m_impl);
    return *this;
}

void Template::extract(Node* node) const
{
    if (!m_impl)
        throw Exception(node, "json::Template: template is not bound !");
    
    m_impl->extract(node);
}

Node* Template::synthetize() const
{
    if (!m_impl)
        throw std::logic_error("json::Template: template is not bound !");
    
    return m_impl->synthetize();
}