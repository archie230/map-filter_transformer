%option c++
%option noyywrap
%option nodefault
%option yylineno

%{
	#include "../parser/parser.hh"
	using token_t = yy::Parser::token_type;
%}

SKIP    [ \t\n\v\r]+
NUM     [-]?([0])|([1-9][0-9]*)

%%

{SKIP}
[()}]                   {return static_cast<token_t>(yytext[0]); }
"map{"                  {return token_t::TMAP;                   }
"filter{"               {return token_t::TFILTER;                }
"element"               {return token_t::TELEM;                  }
"="                     {return token_t::TEQ;                    }
"<"                     {return token_t::TLESS;                  }
">"                     {return token_t::TGR;                    }
"|"                     {return token_t::TOR;                    }
"&"                     {return token_t::TAND;                   }
"+"                     {return token_t::TPLUS;                  }
"-"                     {return token_t::TMINUS;                 }
"*"                     {return token_t::TMUL;                   }
"/"                     {return token_t::TDIV;                   }
"%>%"                   {return token_t::TNEXT;                  }
{NUM}                   {return token_t::TNUM;                   }
.                       {return token_t::TERR;                   }

%%