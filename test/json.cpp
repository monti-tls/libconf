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

#include "lconf/json.h"
#include "lconf/json_template.h"

#include <sstream>

//! You can add custom loading rules as showed below.
//! First, create a class inheriting the json::UserElement class (it
//!   has to reside in the json namespace also).
//! This class must define two functions :
//!   - void extract(Node*), which must process the data in the given node
//!     (you can of course use another template here).
//!   - Node* synthetize(), whih must create a JSON tree representing the owned data.
//! An instance of this class will be created for each variable that must be loaded,
//!    and must provide a constructor taking a reference to the handled type.
//! Secondly, you must specialize the Terminal<> class to use the newly integrated one
//!   (this is really straigthforward as you can see below).
namespace lconf { namespace json
{
    template <typename U, typename V>
    class PairElement : public UserElement
    {
    public:
        PairElement(std::pair<U, V>& ref) :
            m_ref(ref)
        {}
        
        void extract(Node* node) const
        {
            // Type-check the array, throwing a json::Exception instance in case
            //   of a mismatch.
            if (node->type() != Node::Array)
                throw Exception(node, "json::PairElement: type mismatch");
            
            // Downcast safely the node to check its size.
            ArrayNode* arr = node->downcast<ArrayNode>();
            if (arr->size() != 2)
                throw Exception(node, "json::PairElement: size mismatch, expecting 2 for a pair");
            
            // Use another template to read in the array.
            Template tpl;
            tpl.bind_array(m_ref.first);
            tpl.bind_array(m_ref.second);
            tpl.extract(node);
        }
        
        //! Here we don't care about synthetization.
        Node* synthetize() const
        {
            Template tpl;
            tpl.bind_array(m_ref.first);
            tpl.bind_array(m_ref.second);
            return tpl.synthetize();
        }
        
    private:
        std::pair<U, V>& m_ref;
    };
    
    //! Specialize the Terminal<> class to expose the new type to the json::Template
    //!   system.
    template<typename U, typename V>
    class Terminal<std::pair<U, V> > : public PairElement<U, V>
    {
    public:
        Terminal(std::pair<U, V>& ref) : PairElement<U, V>(ref)
        {}
    };
} }

int main()
{
    using namespace lconf;
    using namespace json;
    
    try
    {
        // Set up the JSON input stream (one can use any std::istream).
        std::string val =
        "{\n"
        "   \"a\" : 123,\n"
        "   \"p\" : [1, \"yolo\"]\n"
        "}"
        ;
        std::istringstream ss;
        ss.str(val);
        
        // The variables that we will update w/ the configuration.
        int a;
        std::pair<int, std::string> p;
        
        // Create the template.
        Template tpl = Template()
        .bind("a", a)
        .bind("p", p);
        
        // Match the template and extract conf data from the stream.
        json::extract(tpl, ss);
        
        // Verify that everything is OK :)
        std::cout << "a = " << a << std::endl;
        std::cout << "pair = (" << p.first << ", " << p.second << ")" << std::endl;
        
        // Modify some values and then save configuration
        a = 321;
        p.first = -1;
        p.second = "foo";
        
        // Serialize the template, now the values must have changed
        std::cout << "Serialized (indented version) :" << std::endl;
        json::synthetize(tpl, std::cout);
        std::cout << std::endl << "Compact version : ";
        json::synthetize(tpl, std::cout, false);
        std::cout << std::endl;
    }
    catch(Exception const& exc)
    {
        // Here you can retrieve the offending node :
        Node* offending = exc.node();
        std::cerr << "Exception:[" << offending << "]\n\t" << exc.what() << std::endl;
    }
    catch(std::exception const& exc)
    {
        std::cerr << "Exception:\n\t" << exc.what() << std::endl;
    }
    
    return 0;
}
