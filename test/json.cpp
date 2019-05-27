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
            m_ref(ref),
            m_is_const(false)
        {}

        PairElement(std::pair<U, V> const& ref) :
            m_ref(const_cast<std::pair<U, V>&>(ref)),
            m_is_const(true)
        {}

        void extract(Node* node) const
        {
            if (m_is_const)
                throw Exception(node, "json::PairElement[const]::extract: extracting to const binding");

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

        bool isConst() const
        { return m_is_const; }

    private:
        std::pair<U, V>& m_ref;
        bool m_is_const;
    };

    //! Specialize the Terminal<> class to expose the new type to the json::Template
    //!   system.
    template<typename U, typename V>
    class Terminal<std::pair<U, V> > : public PairElement<U, V>
    {
    public:
        Terminal(std::pair<U, V>& ref) : PairElement<U, V>(ref)
        {}

        Terminal(std::pair<U, V> const& ref) : PairElement<U, V>(ref)
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
        "   \"b\" : 10e-3,\n"
        "   \"p\" : [1, \"yolo\"],\n"
        "   \"v\":  [true, false, true]\n"
        "}"
        ;
        std::istringstream ss;
        ss.str(val);

        // The variables that we will update w/ the configuration.
        int a;
        float b;
        std::pair<int, std::string> p;
        std::vector<bool> v;

        // Create the template.
        Template tpl = Template()
        .bind("a", a)
        .bind("b", b)
        .bind("p", p)
        .bind("v", v);

        // Match the template and extract conf data from the stream.
        json::extract(tpl, ss);

        // Verify that everything is OK :)
        std::cout << "a = " << a << std::endl;
        std::cout << "b = " << b << std::endl;
        std::cout << "pair = (" << p.first << ", " << p.second << ")" << std::endl;
        std::cout << "v = ";
        for (auto val : v)
            std::cout << (val ? "true" : "false") << " ";
        std::cout << std::endl;

        // Modify some values and then save configuration
        a = 321;
        b = 0.000000005;
        p.first = -1;
        p.second = "foo";
        v.push_back(false);

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

    // Testing the const versions

    try
    {
        struct {
            int a;
            float b;
            std::pair<int, std::string> p;
            std::vector<bool> v;
            const char* c;
        } const test = {
            .a = 123,
            .b = 456.759f,
            .p = { 12, "42--C−C" },
            .v = { true, false, true },
            .c = "this is a C string"
        };

        // Create the template.
        Template tpl = Template()
        .bind("a", test.a)
        .bind("b", test.b)
        .bind("p", test.p)
        .bind("v", test.v)
        .bind("s", test.c);

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

    // PODs

    try
    {
        struct __attribute__((packed))
        {
            int a;
            char b[8];
        } const not_really_a_pod[2] = {
            { 123, "abcd" },
            { 456, "efgh" }
        };

        Template tpl = Template()
        .bind("data", ref_as_pod(not_really_a_pod));

        std::cout << "Serialized (indented version) :" << std::endl;
        json::synthetize(tpl, std::cout);
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

    // PODs

    try
    {
        unsigned short raw[4] = { 1, 2, 3, 4 };

        Template tpl = Template()
        .bind("data", ref_as_raw(raw, 4));

        std::cout << "Serialized (indented version) :" << std::endl;
        json::synthetize(tpl, std::cout);

        unsigned short* raw2 = 0;
        std::size_t sz;

        Template tpl2 = Template()
        .bind("data", ref_as_raw(raw2, sz));

        std::istringstream ss("{ \"data\" : \"0400050006000700\" }");
        json::extract(tpl2, ss);

        std::cout << "Read " << sz << " items:";
        for (std::size_t i = 0; i < sz; ++i)
            std::cout << " " << raw2[i];;
        std::cout << std::endl;

        std::cout << "Serialized (indented version) :" << std::endl;
        json::synthetize(tpl2, std::cout);

        delete[] raw2;
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
