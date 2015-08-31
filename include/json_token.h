#ifndef LCONF_JSON_TOKEN_H
#define LCONF_JSON_TOKEN_H

#include <string>

namespace json
{
    class Token
    {
    public:
        enum Type
        {
            Eof,
            Bad,
            LeftBrace,
            RightBrace,
            LeftBracket,
            RightBracket,
            Comma,
            Colon,
            True,
            False,
            Number,
            String
        };
        
        struct Info
        {
            bool empty;
            int line, column;
        };
        
    public:
        Token(Type type = Bad, std::string const& value = "");
        
        Type type() const;
        std::string const& value() const;
        
        void setInfo(Info const& info);
        Info const& info() const;
        
    private:
        Type m_type;
        std::string m_value;
        Info m_info;
    };
}

#endif // LCONF_JSON_TOKEN_H
