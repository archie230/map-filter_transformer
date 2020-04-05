#pragma once

#include "TokenNames.hpp"
#include <fstream>
#include <stack>

namespace AST {

    class INode {
    protected:
        TokenNames token_name_;
        INode* left_;
        INode* right_;
    public:
        explicit INode(TokenNames token_name, INode* left = nullptr, INode* right = nullptr) noexcept
            :
                token_name_(token_name),
                left_(left),
                right_(right)
        {}

        virtual ~INode() {
            delete left_;
            delete right_;
        }

        INode(const INode&) = delete;

        TokenNames GetType() const noexcept { return token_name_; }

        INode* GetLeft() const noexcept { return left_; }

        INode* GetRight() const noexcept { return right_; }

        void SetLeft(INode* node) noexcept { left_ = node; }

        void SetRight(INode* node) noexcept { right_ = node; }

        virtual void dump(std::ostream& os) const {
            switch (token_name_) {
                case ELEM:
                    os << "element";
                    break;
                case PLUS :
                    os << "(";
                    left_ -> dump(os);
                    os << "+";
                    right_ -> dump(os);
                    os << ")";
                    break;
                case MINUS:
                    os << "(";
                    left_ -> dump(os);
                    os << "-";
                    right_ -> dump(os);
                    os << ")";
                    break;
                case MUL  :
                    os << "(";
                    left_ -> dump(os);
                    os << "*";
                    right_ -> dump(os);
                    os << ")";
                    break;
                case DIV  :
                    os << "(";
                    left_ -> dump(os);
                    os << "/";
                    right_ -> dump(os);
                    os << ")";
                    break;
                case GR   :
                    os << "(";
                    left_ -> dump(os);
                    os << ">";
                    right_ -> dump(os);
                    os << ")";
                    break;
                case LESS :
                    os << "(";
                    left_ -> dump(os);
                    os << "<";
                    right_ -> dump(os);
                    os << ")";
                    break;
                case EQ   :
                    os << "(";
                    left_ -> dump(os);
                    os << "=";
                    right_ -> dump(os);
                    os << ")";
                    break;
                case OR   :
                    os << "(";
                    left_ -> dump(os);
                    os << "|";
                    right_ -> dump(os);
                    os << ")";
                    break;
                case AND  :
                    os << "(";
                    left_ -> dump(os);
                    os << "&";
                    right_ -> dump(os);
                    os << ")";
                    break;
            }
        }

        [[nodiscard]]
        virtual AST::INode* copy() const {
            return new AST::INode(token_name_);
        }

        [[nodiscard]]
        AST::INode* deep_copy()  {
            std::stack<AST::INode*> stack;
            std::stack<AST::INode*> new_stack;

            auto new_root = copy();

            stack.push(this);
            new_stack.push(new_root);

            while (!stack.empty()) {
                auto top = stack.top();
                auto new_top = new_stack.top();
                auto left = stack.top() -> GetLeft();
                auto right = stack.top() -> GetRight();

                stack.pop();
                new_stack.pop();

                if (left) {
                    auto new_left = left -> copy();
                    new_top -> SetLeft(new_left);

                    new_stack.push(new_left);
                    stack.push(top -> GetLeft());
                }

                if (right) {
                    auto new_right = right -> copy();
                    new_top -> SetRight(new_right);

                    new_stack.push(new_right);
                    stack.push(top -> GetRight());
                }
            }

            return new_root;
        }
    };

    class NumNode final : public INode {
        int num_;
    public:
        NumNode(int num) noexcept
            :
                INode(TokenNames::NUMBER),
                num_(num)
        {}

        int GetNum() const noexcept { return num_; }

        [[nodiscard]]
        AST::INode* copy() const override {
            return new NumNode(num_);
        }

        void dump(std::ostream& os) const override {
            os << num_;
        }
    };
}