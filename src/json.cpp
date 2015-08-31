#include "json.h"
#include <fstream>

namespace json
{
    Node* parse(std::string const& file)
    {
        std::ifstream fs(file, std::ios::in);
        if (!fs)
            throw std::logic_error("json::parse: unable to open \"" + file + "\"");
        return parse(fs);
    }
    
    Node* parse(std::istream& file)
    {
        Lexer lexer(file);
        Parser parser(lexer);
        return parser.parse();
    }
    
    void extract(Template const& tpl, std::string const& file)
    {
        Node* node = parse(file);
        tpl.extract(node);
        delete node;
    }
    
    void extract(Template const& tpl, std::istream& file)
    {
        Node* node = parse(file);
        tpl.extract(node);
        delete node;
    }
}
