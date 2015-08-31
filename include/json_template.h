#ifndef LCONF_JSON_TEMPLATE_H
#define LCONF_JSON_TEMPLATE_H

#include "json_node.h"
#include <string>
#include <vector>
#include <map>

namespace json
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
        { throw Exception(node, "json::Terminal: direct extraction is not supported for this type"); }
        
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
        { }
        
        Type type() const
        { return Element::Scalar; }
        
        void extract(Node* node) const
        {
            if (node->type() != tp)
                throw Exception(node, "json::Scalar: type mismatch");
            m_ref = node->downcast<N>()->value();
        }
        
        Node* synthetize() const
        { return new N(m_ref); }
        
    private:
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
                throw Exception(node, "json::Vector: type mismatch");
            
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
                throw Exception(node, "json::Map: type mismatch");
            
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
            if (m_impl) throw std::logic_error("json::Template: template is already bound");
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
}

#endif // LCONF_JSON_TEMPLATE_H
