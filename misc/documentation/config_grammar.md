config      → statement*
statement   → block | directive
block       → WORD (WORD)* '{' statement_list '}'
directive   → WORD (WORD)* ';'
