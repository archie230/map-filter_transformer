%language "c++"

%defines
%define api.value.type {AST::INode*}
%define parser_class_name {Parser}

%define parse.error verbose

%skeleton "lalr1.cc"

%param {yy::Driver& driver}

%code requires {
    #include "../AST.hpp"
    #include "../TokenNames.hpp"

    namespace yy {
        class Driver;
    }

    #  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
        do                                                                  \
          if (N)                                                            \
            {                                                               \
              (Current)	       = YYRHSLOC (Rhs, 1);                         \
              (Current)        = YYRHSLOC (Rhs, N);                         \
            }                                                               \
          else                                                              \
            {                                                               \
              (Current)        = YYRHSLOC (Rhs, 0);                         \
            }                                                               \
        while (false)
}

%code {
    #include "../Driver.hpp"
    using namespace AST;

    namespace yy {
    	Parser::token_type
    	yylex(Parser::semantic_type* yylval, Driver& driver);
    }

    bool
    IsBoolOper(INode* node) {
        switch (node -> GetType()) {
                case GR:
                case LESS:
								case EQ:
                case OR:
                case AND:
                        return true;
                default:
                        return false;
        }
    }
}

%token
				TFILTER
				TMAP
				TNUM
				TPLUS
				TMINUS
				TMUL
				TDIV
				TGR
				TLESS
				TEQ
				TOR
				TAND
				TNEXT
				TELEM
				TERR
;

%destructor { delete $$; } <>

/* making grammar unambiguous */
%left TPLUS TMINUS
%left TMUL TDIV
%left TOR  TAND

%start call-chain

%%

call-chain :
  call
| call TNEXT call-chain
;

call:
  TMAP expression '}'
{
	if (!IsBoolOper($expression))
		driver.EmplaceCall(TokenNames::MAP, $expression);
	else
		driver.TypeErr();
}
| TFILTER expression '}'
{
	if (IsBoolOper($expression))
  		driver.EmplaceCall(TokenNames::FILTER, $expression);
	else
		driver.TypeErr();
}
;

expression:
	TELEM { $$ = $1; }
| TNUM  { $$ = $1; }
| binary_expression { $$ = $1; }
;

binary_expression:
	'(' expression[lhs] operation expression[rhs] ')'
{
	// checking type
	if (IsBoolOper($operation)) {
		if ( !(IsBoolOper($lhs)) && !($lhs -> GetType() == TokenNames::NUMBER) && !($lhs -> GetType() == TokenNames::ELEM) )
			driver.TypeErr();

		if ( !(IsBoolOper($rhs)) && !($rhs -> GetType() == TokenNames::NUMBER) && !($rhs -> GetType() == TokenNames::ELEM) )
			driver.TypeErr();
	} else {
		if ( IsBoolOper($lhs) )
			driver.TypeErr();

		if ( IsBoolOper($rhs) )
			driver.TypeErr();
	}

	$operation -> SetLeft($lhs);
	$operation -> SetRight($rhs);
	$$ = $operation;
}
;

operation:
	TPLUS   { $$ = new INode(PLUS ); }
| TMINUS  { $$ = new INode(MINUS); }
| TMUL    { $$ = new INode(MUL  ); }
| TDIV    { $$ = new INode(DIV  ); }
| TGR     { $$ = new INode(GR   ); }
| TLESS   { $$ = new INode(LESS ); }
| TEQ     { $$ = new INode(EQ   ); }
| TAND    { $$ = new INode(AND  ); }
| TOR     { $$ = new INode(OR   ); }
;

%%
namespace yy {
				Parser::token_type
				yylex(Parser::semantic_type* yylval, Driver& driver)
				{
				  return driver.yylex(yylval);
				}

				void yy::Parser::error(const std::string& msg)
				{
					driver.SyntaxErr();
				}
}