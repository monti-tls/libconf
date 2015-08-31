#include "json_token.h"

using namespace json;

Token::Token(Token::Type type, std::string const& value) :
    m_type(type),
    m_value(value)
{
    m_info.empty = true;
}

Token::Type Token::type() const
{ return m_type; }

std::string const& Token::value() const
{ return m_value; }

void Token::setInfo(Token::Info const& info)
{
    m_info = info;
    m_info.empty = false;
}

Token::Info const& Token::info() const
{ return m_info; }
