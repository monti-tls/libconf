#include "json.h"
#include "json_template.h"

#include <sstream>

namespace json
{
    template <typename U, typename V>
    class PairElement : public Element
    {
    public:
        PairElement(std::pair<U, V>& ref) :
            m_ref(ref)
        {}
        
        Element::Type type() const
        { return Element::User; }
        
        void extract(Node* node) const
        {
            if (node->type() != Node::Array)
                throw Exception(node, "json::PairElement: type mismatch");
            
            ArrayNode* arr = node->downcast<ArrayNode>();
            if (arr->size() != 2)
                throw Exception(node, "json::PairElement: size mismatch, expecting 2 for a pair");
            
            Terminal<U> first(m_ref.first);
            first.extract(arr->at(0));
            Terminal<V> second(m_ref.second);
            second.extract(arr->at(1));
        }
        
        Node* synthetize() const
        { return 0; }
        
    private:
        std::pair<U, V>& m_ref;
    };
    
    template<typename U, typename V>
    class Terminal<std::pair<U, V> > : public PairElement<U, V>
    {
    public:
        Terminal(std::pair<U, V>& ref) : PairElement<U, V>(ref)
        {}
    };
}

int main()
{
    using namespace json;
    
    std::string val =
    "{\n"
    "   \"a\" : 123,\n"
    "   \"p\" : [ 1, \"yolo\"]\n"
    "}"
    ;
    
    std::istringstream ss;
    ss.str(val);
    
    int a;
    std::pair<int, std::string> p;
    
    Template tpl = Template()
    .bind("a", a)
    .bind("p", p);
    
    json::extract(tpl, ss);
    
    std::cout << a << std::endl;
    std::cout << p.first << ", " << p.second << std::endl;
    
    return 0;
}
