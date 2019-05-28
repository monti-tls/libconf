/* This file is part of libconf.
 * 
 * Copyright (c) 2015 - 2019, Alexandre Monti
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
#include <sstream>
#include <iomanip>

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
            POD,
            Raw,
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
        virtual bool isConst() const = 0;
        
    public:
        int refs;
    };
    
    //! Terminal element interface (specialized below).
    template <typename T>
    class Terminal : public Element
    {
    public:
        Terminal(T&)
        {}

        Terminal(T const&)
        {}
        
        Type type() const
        { return Element::Scalar; }
        
        void extract(Node* node)
        { throw Exception(node, "json::Terminal::extract: direct extraction is not supported for this type"); }
        
        Node* synthetize() const
        { return 0; }

        bool isConst() const
        { return false; }
    };
    
    //! Generic scalar element.
    template <Node::Type tp, typename N, typename T>
    class Scalar : public Element
    {
    public:
        Scalar(T& ref) :
            m_ref(ref),
            m_is_const(false)
        {}

        Scalar(T const& ref) :
            m_ref(const_cast<T&>(ref)),
            m_is_const(true)
        {}
        
        Type type() const
        { return Element::Scalar; }
        
        void extract(Node* node) const
        {
            if (m_is_const)
                throw Exception(node, "json::Scalar[const]::extract extracting to const binding");

            if (node->type() != tp)
                throw Exception(node, "json::Scalar::extract: expecting a node of type " + Node::typeName(tp));
            m_ref = node->downcast<N>()->value();
        }
        
        Node* synthetize() const
        { return new N(m_ref); }

        bool isConst() const
        { return false; }
        
    protected:
        T& m_ref;
        bool m_is_const;
    };

    //! Generic POD element.
    template <typename T>
    class POD : public Element
    {
    public:
        POD(T& ref) :
            m_ref(ref),
            m_is_const(false)
        {}

        POD(T const& ref) :
            m_ref(const_cast<T&>(ref)),
            m_is_const(true)
        {}

        Type type() const
        { return Element::POD; }

        void extract(Node* node) const
        {
            if (m_is_const)
                throw Exception(node, "json::POD[const]::extract: extracting to const binding");

            if (node->type() != Node::String)
                throw Exception(node, "json::POD::extract: expecting a string node");

            std::string as_hex = node->downcast<json::StringNode>()->value();
            if (as_hex.size() % 2 != 0 || as_hex.size() / 2 != sizeof(T))
                throw Exception(node, "json::POD::extract: bad buffer size");

            uint8_t* data = reinterpret_cast<uint8_t*>(&m_ref);
            for (std::size_t byte = 0; byte < as_hex.size()/2; ++byte)
            {
                std::istringstream ss(as_hex.substr(2*byte, 2));
                ss >> std::hex;
                int value;
                ss >> value;
                data[byte] = static_cast<uint8_t>(value & 0xFF);
            }
        }

        Node* synthetize() const
        {
            std::ostringstream ss;
            uint8_t* data = reinterpret_cast<uint8_t*>(&m_ref);

            for (std::size_t byte = 0; byte < sizeof(T); ++byte)
                ss << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(data[byte]);

            return new StringNode(ss.str());
        }

        bool isConst() const
        { return m_is_const; }

    private:
        T& m_ref;
        bool m_is_const;
    };

    //! Used to tag types as POD
    template <typename T>
    struct tag_as_pod_impl
    {
    public:
        tag_as_pod_impl(T& ref) : ref(ref)
        {}

        T& ref;
    };

    //! Used to tag types as POD (const)
    template <typename T>
    struct tag_as_const_pod_impl
    {
    public:
        tag_as_const_pod_impl(T const& ref) : ref(ref)
        {}

        T const& ref;
    };

    //! Used to tag types as POD
    template<typename T>
    static tag_as_pod_impl<T> ref_as_pod(T& ref)
    { return tag_as_pod_impl<T>(ref); }

    template<typename T>
    static tag_as_const_pod_impl<T> ref_as_pod(T const& ref)
    { return tag_as_const_pod_impl<T>(ref); }


    //! Generic POD element.
    template <typename T>
    class Raw : public Element
    {
    public:
        Raw(T*& ptr, std::size_t& size) :
            m_ptr(&ptr),
            m_size(&size),
            m_is_const(false)
        {}

        Raw(T const* ptr, std::size_t size) :
            m_ptr(new T*(const_cast<T*>(ptr))),
            m_size(new std::size_t(size)),
            m_is_const(true)
        {}

        ~Raw()
        {
            if (m_is_const)
            {
                delete m_ptr;
                delete m_size;
            }
        }

        Type type() const
        { return Element::Raw; }

        void extract(Node* node) const
        {
            if (m_is_const)
                throw Exception(node, "json::Raw[const]::extract: extracting to const binding");

            if (node->type() != Node::String)
                throw Exception(node, "json::Raw::extract: expecting a string node");

            if (*m_ptr != 0)
                throw Exception(node, "json::Raw::extract: target memory is already allocated");

            std::string as_hex = node->downcast<json::StringNode>()->value();
            if (as_hex.size() % 2 != 0)
                throw Exception(node, "json::Raw::extract: bad buffer size");

            *m_size = as_hex.size() / (2 * sizeof(T));
            *m_ptr = new T[*m_size];

            for (std::size_t i = 0; i < *m_size; ++i)
            {
                uint8_t* data = reinterpret_cast<uint8_t*>(&(*m_ptr)[i]);

                for (std::size_t byte = 0; byte < sizeof(T); ++byte)
                {
                    std::string nibbles = as_hex.substr(2*i * sizeof(T) + 2*byte, 2);

                    std::istringstream ss(nibbles);
                    ss >> std::hex;
                    int value;
                    ss >> value;

                    data[byte] = static_cast<uint8_t>(value & 0xFF);
                }
            }
        }

        Node* synthetize() const
        {
            std::ostringstream ss;

            for (std::size_t i = 0; i < *m_size; ++i)
            {
                uint8_t* data = reinterpret_cast<uint8_t*>(&(*m_ptr)[i]);
                for (std::size_t byte = 0; byte < sizeof(T); ++byte)
                    ss << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(data[byte]);
            }

            return new StringNode(ss.str());
        }

        bool isConst() const
        { return m_is_const; }

    private:
        T** m_ptr;
        std::size_t* m_size;
        bool m_is_const;
    };

    //! Used to tag types as RAW
    template <typename T>
    struct tag_as_raw_impl
    {
    public:
        tag_as_raw_impl(T*& ptr, std::size_t& size) : ptr(ptr), size(size)
        {}

        T*& ptr;
        std::size_t& size;
    };

    //! Used to tag types as RAW (const)
    template <typename T>
    struct tag_as_const_raw_impl
    {
    public:
        tag_as_const_raw_impl(T const* ptr, std::size_t size) : ptr(ptr), size(size)
        {}

        T const* ptr;
        std::size_t size;
    };

    //! Used to tag types as RAW
    template<typename T>
    static tag_as_raw_impl<T> ref_as_raw(T*& ptr, std::size_t& size)
    { return tag_as_raw_impl<T>(ptr, size); }

    template<typename T>
    static tag_as_const_raw_impl<T> ref_as_raw(T const* ptr, std::size_t size)
    { return tag_as_const_raw_impl<T>(ptr, size); }
    
    //! Generic vector element.
    template <typename T>
    class Vector : public Element
    {
    public:
        Vector(std::vector<T>& ref) :
            m_ref(ref),
            m_is_const(false)
        {}

        Vector(std::vector<T> const& ref) :
            m_ref(const_cast<std::vector<T>&>(ref)),
            m_is_const(true)
        {}
        
        Type type() const
        { return Element::Vector; }
        
        void extract(Node* node) const
        {
            if (m_is_const)
                throw Exception(node, "json::Vector[const]::extract: extracting to const binding");

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

        bool isConst() const
        { return m_is_const; }
        
    private:
        std::vector<T>& m_ref;
        bool m_is_const;
    };
    
    //! Generic map element.
    template <typename T>
    class Map : public Element
    {
    public:
        Map(std::map<std::string, T>& ref) :
            m_ref(ref),
            m_is_const(false)
        {}

        Map(std::map<std::string, T> const& ref) :
            m_ref(const_cast<std::map<std::string, T>&>(ref)),
            m_is_const(true)
        {}
        
        Type type() const
        { return Element::Map; }
        
        void extract(Node* node) const
        {
            if (m_is_const)
                throw Exception(node, "json::Map[const]::extract: extracting to const binding");

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

        bool isConst() const
        { return m_is_const; }
        
    private:
        std::map<std::string, T>& m_ref;
        bool m_is_const;
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

        Terminal(int const& ref) : Scalar(ref)
        {}
    };

    template <>
    class Terminal<unsigned int> : public Scalar<Node::Number, NumberNode, unsigned int>
    {
    public:
        Terminal(unsigned int& ref) : Scalar(ref)
        {}

        Terminal(unsigned int const& ref) : Scalar(ref)
        {}
    };

    template <>
    class Terminal<std::size_t> : public Scalar<Node::Number, NumberNode, std::size_t>
    {
    public:
        Terminal(std::size_t& ref) : Scalar(ref)
        {}

        Terminal(std::size_t const& ref) : Scalar(ref)
        {}
    };
    
    template <>
    class Terminal<float> : public Scalar<Node::Number, NumberNode, float>
    {
    public:
        Terminal(float& ref) : Scalar(ref)
        {}

        Terminal(float const& ref) : Scalar(ref)
        {}
    };
    
    template <>
    class Terminal<double> : public Scalar<Node::Number, NumberNode, double>
    {
    public:
        Terminal(double& ref) : Scalar(ref)
        {}

        Terminal(double const& ref) : Scalar(ref)
        {}
    };
    
    template <>
    class Terminal<bool> : public Scalar<Node::Boolean, BooleanNode, bool>
    {
    public:
        Terminal(bool& ref) : Scalar(ref)
        {}

        Terminal(bool const& ref) : Scalar(ref)
        {}
    };
    
    template <>
    class Terminal<std::string> : public Scalar<Node::String, StringNode, std::string>
    {
    public:
        Terminal(std::string& ref) : Scalar(ref)
        {}

        Terminal(std::string const& ref) : Scalar(ref)
        {}
    };

    template <>
    class Terminal<const char*> : public Scalar<Node::String, StringNode, std::string>
    {
    public:
        Terminal(const char* ptr) :
            Scalar(static_cast<std::string const&>(m_str)),
            m_str(ptr)
        {}

    private:
        std::string m_str;
    };
    
    template <typename T>
    class Terminal<std::vector<T> > : public Vector<T>
    {
    public:
        Terminal(std::vector<T>& ref) : Vector<T>(ref)
        {}

        Terminal(std::vector<T> const& ref) : Vector<T>(ref)
        {}
    };
    
    template <typename T>
    class Terminal<std::map<std::string, T> > : public Map<T>
    {
    public:
        Terminal(std::map<std::string, T>& ref) : Map<T>(ref)
        {}

        Terminal(std::map<std::string, T> const& ref) : Map<T>(ref)
        {}
    };

    template <typename T>
    class Terminal<tag_as_pod_impl<T> > : public POD<T>
    {
    public:
        Terminal(tag_as_pod_impl<T> ref) : POD<T>(ref.ref)
        {}
    };

    template <typename T>
    class Terminal<tag_as_const_pod_impl<T> > : public POD<T>
    {
    public:
        Terminal(tag_as_const_pod_impl<T> ref) : POD<T>(ref.ref)
        {}
    };

    template <typename T>
    class Terminal<tag_as_raw_impl<T> > : public Raw<T>
    {
    public:
        Terminal(tag_as_raw_impl<T> ref) : Raw<T>(ref.ptr, ref.size)
        {}
    };

    template <typename T>
    class Terminal<tag_as_const_raw_impl<T> > : public Raw<T>
    {
    public:
        Terminal(tag_as_const_raw_impl<T> ref) : Raw<T>(ref.ptr, ref.size)
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
        bool isConst() const;
        
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
        bool isConst() const;
        
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
        Template(T const& const_ref) :
            m_impl(0)
        { bind(const_ref); }
        
        template <typename T>
        Template& bind(T& ref)
        {
            if (m_impl) throw std::logic_error("json::Template::bind: template is already bound");
            m_impl = new Terminal<T>(ref);
            return *this;
        }
        
        template <typename T>
        Template& bind(T const& const_ref)
        {
            if (m_impl) throw std::logic_error("json::Template::bind: template is already bound");
            m_impl = new Terminal<T>(const_ref);
            return *this;
        }
        
        Template& bind(std::string const& name, Template const& tpl);
        Template& bind_array(Template const& tpl);

        bool bound() const;
        
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
            m_ref(new std::vector<bool>::reference(ref)),
            m_is_const(false)
        {}

        ~Scalar()
        { delete m_ref; }
        
        Type type() const
        { return Element::Scalar; }
        
        void extract(Node* node) const
        {
            if (m_is_const)
                throw Exception(node, "json::Scalar[const]::extract: extracting to const binding");

            if (node->type() != tp)
                throw Exception(node, "json::Scalar::extract: expecting a node of type " + Node::typeName(tp));
            *m_ref = node->downcast<N>()->value();
        }
        
        Node* synthetize() const
        { return new N(*m_ref); }

        bool isConst() const
        { return m_is_const; }
        
    protected:
        std::vector<bool>::reference* m_ref;
        bool m_is_const;
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
            m_ref(ref),
            m_is_const(false)
        {}

        Vector(std::vector<bool> const& ref) :
            m_ref(const_cast<std::vector<bool>&>(ref)),
            m_is_const(true)
        {}
        
        Type type() const
        { return Element::Vector; }
        
        void extract(Node* node) const
        {
            if (m_is_const)
                throw Exception(node, "json::Vector[const]::extract: extracting to const binding");

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

        bool isConst() const
        { return m_is_const; }
        
    private:
        std::vector<bool>& m_ref;
        bool m_is_const;
    };
} }

#endif // LCONF_JSON_TEMPLATE_H
