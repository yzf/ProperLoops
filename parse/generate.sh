lex -o ../src/lex.cc lex.l
yacc --defines=../src/parse.h -o ../src/parse.cc yacc.y
