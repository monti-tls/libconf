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

#ifndef LCONF_JSON_TEMPLATE_H
#define LCONF_JSON_TEMPLATE_H

#include "lconf/json_node.h"
#include <string>
#include <vector>
#include <map>

namespace lconf { namespace json
{
    //! Common exception class to keep track of offending nodes.
    class Exception : public std::logic_error
    {
    public:
        Exception(Node* node, std::string const& what);
        Node* node() const;
        
    private:
        Node* m_node;
    };
    
    //! Common abstract template element interface.
    class Element
    {
    public:
        enum Type
        {
            User,
            Scalar,
            Vector,
            Map,
            Object,
            Array
        };
        
    public:
        Element();
        virtual ~Element();
        virtual Type type() const = 0;
        virtual void extract(Node* node) const = 0;
        virtual Node* synthetize() const = 0;
        
    public:
        int refs;
    };
    
    //! Terminal element interface (specialized below).
    template <typename T>
    class Terminal : public Element
    {
    public:
        Terminal(T&)
        { }
        
        Type type() const
        { return Element::Scalar; }
        
        void extract(Node* node)
        { throw Exception(node, "json::Terminal::extract: direct extraction is not supported for this type"); }
        
        Node* synthetize() const
        { return 0; }
    };
    
    //! Generic scalar element.
    template <Node::Type tp, typename N, typename T>
    class Scalar : public Element
    {
    public:
        Scalar(T& ref) :
            m_ref(ref)
        {}
        
        Type type() const
        { return Element::Scalar; }
        
        void extract(Node* node) const
        {
            if (node->type() != tp)
                throw Exception(node, "json::Scalar::extract: expecting a node of type " + Node::typeName(tp));
            m_ref = node->downcast<N>()->value();
        }
        
        Node* synthetize() const
        { return new N(m_ref); }
        
    protected:
        T& m_ref;
    };
    
    //! Generic vector element.
    template <typename T>
    class Vector : public Element
    {
    public:
        Vector(std::vector<T>& ref) :
            m_ref(ref)
        {}
        
        Type type() const
        { return Element::Vector; }
        
        void extract(Node* node) const
        {
            if (node->type() != Node::Array)
                throw Exception(node, "json::Vector::extract: expecting an array node");
            
            ArrayNode* arr = node->downcast<ArrayNode>();
            
            m_ref.clear();
            m_ref.reserve(arr->size());
            for (unsigned int i = 0; i < arr->size(); ++i)
            {
                T value;
                Terminal<T> term(value);
                term.extract(arr->at(i));
                m_ref.push_back(value);
            }
        }
        
        Node* synthetize() const
        {
            ArrayNode* arr = new ArrayNode();
            for (unsigned int i = 0; i < m_ref.size(); ++i)
            {
                Terminal<T> term(m_ref[i]);
                arr->impl().push_back(term.synthetize());
            }
            return arr;
        }
        
    private:
        std::vector<T>& m_ref;
    };
    
    //! Generic map element.
    template <typename T>
    class Map : public Element
    {
    public:
        Map(std::map<std::string, T>& ref) :
            m_ref(ref)
        {}
        
        Type type() const
        { return Element::Map; }
        
        void extract(Node* node) const
        {
            if (node->type() != Node::Object)
                throw Exception(node, "json::Map::extract: expecting an object node");
            
            ObjectNode* obj = node->downcast<ObjectNode>();
            
            m_ref.clear();
            for (std::map<std::string, Node*>::iterator it = obj->impl().begin();
                it != obj->impl().end(); ++it)
            {
                T value;
                Terminal<T> term(value);
                term.extract(it->second);
                m_ref[it->first] = value;
            }
        }
        
        Node* synthetize() const
        {
            ObjectNode* obj = new ObjectNode();
            for (typename std::map<std::string, T>::iterator it = m_ref.begin();
                it != m_ref.end(); ++it)
            {
                T value;
                Terminal<T> term(it->second);
                obj->impl()[it->first] = term.synthetize();
            }
            return obj;
        }
        
    private:
        std::map<std::string, T>& m_ref;
    };
    
    //! Below are the specializations for the Terminal
    //!   element class.
    //! Instances that are not specialized here will systematically trigger errors
    //!   (see beginning of this file).
    
    template <>
    class Terminal<int> : public Scalar<Node::Number, NumberNode, int>
    {
    public:
        Terminal(int& ref) : Scalar(ref)
        {}
    };
    
    template <>
    class Terminal<float> : public Scalar<Node::Number, NumberNode, float>
    {
    public:
        Terminal(float& ref) : Scalar(ref)
        {}
    };
    
    template <>
    class Terminal<bool> : public Scalar<Node::Boolean, BooleanNode, bool>
    {
    public:
        Terminal(bool& ref) : Scalar(ref)
        {}
    };
    
    template <>
    class Terminal<std::string> : public Scalar<Node::String, StringNode, std::string>
    {
    public:
        Terminal(std::string& ref) : Scalar(ref)
        {}
    };
    
    template <typename T>
    class Terminal<std::vector<T> > : public Vector<T>
    {
    public:
        Terminal(std::vector<T>& ref) : Vector<T>(ref)
        {}
    };
    
    template <typename T>
    class Terminal<std::map<std::string, T> > : public Map<T>
    {
    public:
        Terminal(std::map<std::string, T>& ref) : Map<T>(ref)
        {}
    };
    
    //! An object element class.
    class Object : public Element
    {
    public:
        Object();
        ~Object();
        
        void bind(std::string const& name, Element* elem);
        Type type() const;
        void extract(Node* node) const;
        Node* synthetize() const;
        
    private:
        std::map<std::string, Element*> m_elements;
    };
    
    //! An array element class.
    class Array : public Element
    {
    public:
        Array();
        ~Array();
        
        void bind(Element* elem);
        Type type() const;
        void extract(Node* node) const;
        Node* synthetize() const;
        
    private:
        std::vector<Element*> m_elements;
    };
    
    //! A small helper Element overloaded class
    //!   (that just defines type() const).
    class UserElement : public Element
    {
    public:
        Type type() const
        { return Element::User; }
    };
    
    //! The final JSON template class.
    class Template
    {
    public:
        Template();
        Template(Template& cpy);
        Template(Template const& cpy);
        ~Template();
        
        Template& operator=(Template const& cpy);
        
        template <typename T>
        Template(T& ref) :
            m_impl(0)
        { bind(ref); }
        
        template <typename T>
        Template& bind(T& ref)
        {
            if (m_impl) throw std::logic_error("json::Template::bind: template is already bound");
            m_impl = new Terminal<T>(ref);
            return *this;
        }
        
        Template& bind(std::string const& name, Template const& tpl);
        Template& bind_array(Template const& tpl);
        
        void extract(Node* node) const;
        Node* synthetize() const;
        
    private:
        Element* m_impl;
    };

    //!
    //! Below are the hacks to handle the std::vector<bool> specialization,
    //!   for which the operator[] does not return a bool& but a special
    //!   proxy class
    //!

    //! Generic scalar element.
    //! We must use the hack of allocating a copy of the vector::reference,
    //!   because if we store by value we violate the constness of extract,
    //!   and if we store it by reference there is an "always true" bug
    template <Node::Type tp, typename N>
    class Scalar<tp, N, std::vector<bool>::reference> : public Element
    {
    public:
        Scalar(std::vector<bool>::reference& ref) :
            m_ref(new std::vector<bool>::reference(ref))
        {}

        ~Scalar()
        { delete m_ref; }
        
        Type type() const
        { return Element::Scalar; }
        
        void extract(Node* node) const
        {
            if (node->type() != tp)
                throw Exception(node, "json::Scalar::extract: expecting a node of type " + Node::typeName(tp));
            *m_ref = node->downcast<N>()->value();
        }
        
        Node* synthetize() const
        { return new N(*m_ref); }
        
    protected:
        std::vector<bool>::reference* m_ref;
    };
    
    template <>
    class Terminal<std::vector<bool>::reference> : public Scalar<Node::Boolean, BooleanNode, std::vector<bool>::reference>
    {
    public:
        Terminal(std::vector<bool>::reference ref) : Scalar(ref)
        {}
    };

    //! Generic vector element.
    template <>
    class Vector<bool> : public Element
    {
    public:
        Vector(std::vector<bool>& ref) :
            m_ref(ref)
        {}
        
        Type type() const
        { return Element::Vector; }
        
        void extract(Node* node) const
        {
            if (node->type() != Node::Array)
                throw Exception(node, "json::Vector::extract: expecting an array node");
            
            ArrayNode* arr = node->downcast<ArrayNode>();
            
            m_ref.clear();
            m_ref.reserve(arr->size());
            for (unsigned int i = 0; i < arr->size(); ++i)
            {
                bool value;
                Terminal<bool> term(value);
                term.extract(arr->at(i));
                m_ref.push_back(value);
            }
        }
        
        Node* synthetize() const
        {
            ArrayNode* arr = new ArrayNode();
            for (unsigned int i = 0; i < m_ref.size(); ++i)
            {
                Terminal<std::vector<bool>::reference> term(m_ref[i]);
                arr->impl().push_back(term.synthetize());
            }
            return arr;
        }
        
    private:
        std::vector<bool>& m_ref;
    };
} }

#endif // LCONF_JSON_TEMPLATE_H
