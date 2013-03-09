%{
%}

DIGIT    	[0-9]
ID       	([a-z]|[A-Z]|_)([a-z]|[A-Z]|[0-9]|_)*

%%
("//"([ \t]|[a-z]|[A-Z]|[0-9])*)|("/*"([a-z]|[A-Z]|[0-9)|[ \t\n])*"*/")		{printf("<Comments	%s>\n",yytext);}
for|if|else|do|while|int|char|struct|return|switch|typedef|unsigned|void|float|goto	{printf( "<A keyword: 	%s>\n", yytext );}
{DIGIT}+    	{printf( "<Integer constants: 	%s (%d)>\n", yytext,atoi( yytext ) );}
{ID}        {printf( "<Identifier: 	%s>\n", yytext );}
"+"|"-"|"/"|"%"|"*"|"|"|"="|"=="|"&"|"&&"|"<"|">"|"<="|">="|"+="|"-="|"*="|"/="|"++"|"--"	 {printf( "<An operator: 	%s>\n", yytext );}
"#define"|"#include"	{printf("<preprocessors:	%s>\n",yytext);}
[ \t\n]+          {printf( "<WhiteSpaces	%s>\n", yytext );/* eat up whitespace */}
","|";"|"{"|"}"|"\."|"\("|"\)"			{printf("<special symbols	%s>\n",yytext);}
.           {printf(" <Unrecognized character: %s>\n", yytext );}
%%
int main(){
yylex();
}
