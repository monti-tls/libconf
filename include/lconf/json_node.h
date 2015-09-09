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

#ifndef LCONF_JSON_NODE_H
#define LCONF_JSON_NODE_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

namespace lconf { namespace json
{
    class NumberNode;
    class BooleanNode;
    class StringNode;
    class ObjectNode;
    class ArrayNode;
    
    class Node
    {
        //! Needed to access private members of *Node from
        //!   M_serialize() and M_multiline().
        friend class ObjectNode;
        friend class ArrayNode;
    public:
        enum Type
        {
            Number,
            Boolean,
            String,
            Object,
            Array
        };
        
    protected:
        Node() {}
        
    public:
        virtual ~Node() {}
        
        //! Get the type of this node.
        virtual Type type() const = 0;
        //! Get the type string of this node.
        static std::string typeName(Type type);
        //! Serialize the JSON tree whose root is this node to the
        //!   given output stream.
        //! If indent == false, no indentation is outputted
        //!   (and you get a compact, single-line output).
        void serialize(std::ostream& out, bool indent = true) const;
        
        template <typename T>
        T* downcast()
        { return M_downcast((T*) 0); }
        
    protected:
        virtual void M_serialize(std::ostream& out, int level, bool indent) const = 0;
        virtual bool M_multiline() const = 0;
        
        template <typename T>
        T* M_downcast(T*)
        { return 0; }
        
        NumberNode* M_downcast(NumberNode*)
        { return M_safeCast<Number, NumberNode>(); }
        
        BooleanNode* M_downcast(BooleanNode*)
        { return M_safeCast<Boolean, BooleanNode>(); }
        
        StringNode* M_downcast(StringNode*)
        { return M_safeCast<String, StringNode>(); }
        
        ObjectNode* M_downcast(ObjectNode*)
        { return M_safeCast<Object, ObjectNode>(); }
        
        ArrayNode* M_downcast(ArrayNode*)
        { return M_safeCast<Array, ArrayNode>(); }
        
        template <Type tp, typename T>
        T* M_safeCast()
        {
            if (type() != tp) return 0;
            return (T*) this;
        }
    };
    
    class NumberNode : public Node
    {
    public:
        NumberNode(float value);
        
        Type type() const;
        float value() const;
        
    private:
        void M_serialize(std::ostream& out, int level, bool indent) const;
        bool M_multiline() const;
        
    private:
        float m_value;
    };
    
    class BooleanNode : public Node
    {
    public:
        BooleanNode(bool value);
        
        Type type() const;
        bool value() const;
        
    private:
        void M_serialize(std::ostream& out, int level, bool indent) const;
        bool M_multiline() const;
        
    private:
        bool m_value;
    };
    
    class StringNode : public Node
    {
    public:
        StringNode(std::string const& value);
        
        Type type() const;
        std::string const& value() const;
        
    private:
        void M_serialize(std::ostream& out, int level, bool indent) const;
        bool M_multiline() const;
        
    private:
        std::string m_value;
    };
    
    class ObjectNode : public Node
    {
    public:
        ObjectNode();
        ~ObjectNode();
        
        Type type() const;
        bool exists(std::string const& key);
        Node*& get(std::string const& key);
        Node* get(std::string const& key) const;
        std::map<std::string, Node*>& impl();
        std::map<std::string, Node*> const& impl() const;
        
    private:
        void M_serialize(std::ostream& out, int level, bool indent) const;
        bool M_multiline() const;
        
    private:
        std::map<std::string, Node*> m_impl;
    };
    
    class ArrayNode : public Node
    {
    public:
        ArrayNode();
        ~ArrayNode();
        
        Type type() const;
        size_t size() const;
        Node*& at(size_t i);
        Node* at(size_t i) const;
        std::vector<Node*>& impl();
        std::vector<Node*> const& impl() const;
        
    private:
        void M_serialize(std::ostream& out, int level, bool indent) const;
        bool M_multiline() const;
        
    private:
        std::vector<Node*> m_impl;
    };
} }

#endif // LCONF_JSON_NODE_H
