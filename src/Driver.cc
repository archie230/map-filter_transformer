#include "Driver.hpp"
#include <stack>

yy::token_t yy::Driver::yylex(yy::Parser::semantic_type *yylval) {
    auto tt = static_cast<token_t>(plex_.yylex());

    while(tt == token_t::TERR) {
        ++syntax_err_;
        tt = static_cast<token_t>(plex_.yylex());
    }

    switch(tt) {
        case token_t::TNUM: {
            int val = 0;

            try {
                val = std::stoi(std::string(plex_.YYText(), plex_.YYLeng()));
            } catch(const std::out_of_range&) {
                std::cerr << "  error:" << " integer overflow: "
                          << plex_.YYText() << std::endl;
                ++syntax_err_;
                *yylval = nullptr;
                break;
            }

            *yylval = new AST::NumNode(val);
            break;
        }

        case token_t::TELEM:
            *yylval = new AST::INode(TokenNames::ELEM);
            break;

        default:
            *yylval = nullptr;
    }
    return tt;
}

static AST::INode* rebind_element(AST::INode* tree, AST::INode* elem);
static AST::INode* merge_filters(AST::INode* lhs, AST::INode* rhs);

void yy::Driver::Transform() {
    AST::INode* result_mapcall = nullptr;
    AST::INode* result_filtercall = nullptr;

    for (auto [type, expression] : calls_) {
        assert(type == TokenNames::MAP || type == TokenNames::FILTER);
        if (type == TokenNames::MAP) {
            auto save = result_mapcall;
            result_mapcall = rebind_element(expression, result_mapcall);
            delete save;
        }
        else {
            expression = rebind_element(expression, result_mapcall);
            result_filtercall = merge_filters(expression, result_filtercall);
        }
    }
    
    calls_.clear();
    calls_.emplace_back(TokenNames::FILTER, result_filtercall);
    calls_.emplace_back(TokenNames::MAP, result_mapcall);
}

void yy::Driver::Dump(std::ostream &os) {
    assert(calls_.size() == 2);
    assert(calls_[0].first == TokenNames::FILTER);
    assert(calls_[1].first == TokenNames::MAP);

    os << "filter{";
    if (calls_[0].second)
        calls_[0].second -> dump(os);
    else
        os << "(1=1)";
    os << "}%>%";

    os << "map{";
    if (calls_[1].second)
        calls_[1].second -> dump(os);
    else
        os << "element";
    os << "}" << std::endl;
}

AST::INode* rebind_element(AST::INode* tree, AST::INode* elem) {
    if (elem == nullptr || elem -> GetType() == TokenNames::ELEM) {
        return tree;
    } else {
        if (tree -> GetType() == TokenNames::ELEM) {
            delete tree;
            return elem -> copy();
        }

        std::stack<AST::INode*> stack;
        stack.push(tree);

        AST::INode* top = nullptr;
        AST::INode* left = nullptr;
        AST::INode* right = nullptr;

        while (!stack.empty())
        {
            top = stack.top();
            left = stack.top() -> GetLeft();
            right = stack.top() -> GetRight();

            stack.pop();

            if (left) {
                if (left -> GetType() == TokenNames::ELEM) {
                    top -> SetLeft(elem -> deep_copy());
                    delete left;
                } else
                    stack.push(left);
            }

            if (right) {
                if (right -> GetType() == TokenNames::ELEM) {
                    top -> SetRight(elem -> deep_copy());
                    delete right;
                } else
                    stack.push(right);
            }
        }
        return tree;
    }
}

AST::INode* merge_filters(AST::INode* lhs, AST::INode* rhs) {
    if (rhs != nullptr && lhs != nullptr)
        return new AST::INode(TokenNames::AND, lhs, rhs);
    else {
        assert(lhs || rhs);
        if (lhs)
            return lhs;
        else
            return rhs;
    }
}