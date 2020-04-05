#pragma once

#include <FlexLexer.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <cassert>

#include "parser/parser.hh"
#include "AST.hpp"
#include "TokenNames.hpp"

namespace yy {

    using token_t = Parser::token_type;
    using call_t = std::pair<TokenNames, AST::INode*>;
    /**
     * parsing driver
     */
    class Driver final {
        yyFlexLexer         plex_;
        uint32_t            syntax_err_;
        uint32_t            type_err_;
        std::vector<call_t> calls_;
    public:

        explicit Driver(std::istream *is)
                :
                plex_(is),
                syntax_err_(0),
                type_err_(0),
                calls_()
        {}

        ~Driver() {
            for (auto& elem : calls_)
                delete elem.second;
        }

        void SyntaxErr() noexcept { ++syntax_err_; }
        void TypeErr() noexcept { ++type_err_;}

        uint32_t SyntaxErrNum() const noexcept { return syntax_err_; }
        uint32_t TypeErrNum() const noexcept { return type_err_;}

        token_t yylex(Parser::semantic_type *yylval);

        int parse() {
            Parser parser(*this);
            auto retval = parser.parse();

            return retval;
        }

        void EmplaceCall(TokenNames token_name, AST::INode* node) {
            calls_.emplace_back(token_name, node);
        }

        void Transform();

        void Dump(std::ostream& ofs);

    };

}