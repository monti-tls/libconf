#ifndef LCONF_JSON_H
#define LCONF_JSON_H

#include "json_token.h"
#include "json_lexer.h"
#include "json_node.h"
#include "json_parser.h"
#include "json_template.h"
#include <string>
#include <iostream>

namespace json
{
    Node* parse(std::string const& file);
    Node* parse(std::istream& file);
    void extract(Template const& tpl, std::string const& file);
    void extract(Template const& tpl, std::istream& file);
}

#endif // LCONF_JSON_H
