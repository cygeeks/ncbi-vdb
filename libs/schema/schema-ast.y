/*===========================================================================
*
*                            PUBLIC DOMAIN NOTICE
*               National Center for Biotechnology Information
*
*  This software/database is a "United States Government Work" under the
*  terms of the United States Copyright Act.  It was written as part of
*  the author's official duties as a United States Government employee and
*  thus cannot be copyrighted.  This software/database is freely available
*  to the public for use. The National Library of Medicine and the U.S.
*  Government have not placed any restriction on its use or reproduction.
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the NLM and the U.S.
*  Government do not and cannot warrant the performance or results that
*  may be obtained by using this software or data. The NLM and the U.S.
*  Government disclaim all warranties, express or implied, including
*  warranties of performance, merchantability or fitness for any particular
*  purpose.
*
*  Please cite the author in any work or product based on this material.
*
* ===========================================================================
*
*/

%{
    #define YYDEBUG 1

    #include <stdio.h>

    #include "ASTBuilder.hpp"
    using namespace ncbi::SchemaParser;

    #include "schema-ast-tokens.h"

    #define AST_lex NextToken
    static int NextToken ( YYSTYPE* p_token, ParseTreeScanner& p_sb )
    {
        return p_sb . NextToken ( p_token -> tok );
    }

    void AST_error ( void* parser, ASTBuilder& p_builder, ParseTreeScanner&, const char* msg )
    {
        /*TODO: retrieve name/location data from the scanner, add to the report */
        p_builder . ReportError ( "%s", msg );
    }

%}

%name-prefix "AST_"
%parse-param { AST*& p_ast }
%parse-param { ASTBuilder& p_builder }
%param { ParseTreeScanner& p_sb }

 /*%define api.value.type { const Token* }*/

%union {
  const Token*  tok;
  AST*          node;
  AST_FQN*      fqn;
  AST_Expr*     expr;
  AST_ParamSig* paramSig;
  bool          boolean;
}

%define parse.error verbose

%define api.pure full

 /* !!! Keep token declarations in synch with schema-grammar.y */

%token END_SOURCE 0 "end of source"

%token UNRECOGNIZED

%token ELLIPSIS
%token INCREMENT

%token DECIMAL
%token OCTAL
%token HEX
%token FLOAT
%token EXP_FLOAT
%token STRING
%token ESCAPED_STRING

%token IDENTIFIER_1_0
%token PHYSICAL_IDENTIFIER_1_0
%token VERSION

/* unterminated strings are dealt with by flex */
%token UNTERM_STRING
%token UNTERM_ESCAPED_STRING

%token VERS_1_0 /* recognized under special flex state */

%token KW___no_header
%token KW___row_length
%token KW___untyped
%token KW_alias
%token KW_column
%token KW_const
%token KW_control
%token KW_database
%token KW_decode
%token KW_default
%token KW_encode
%token KW_extern
%token KW_false
%token KW_fmtdef
%token KW_function
%token KW_include
%token KW_limit
%token KW_physical
%token KW_read
%token KW_readonly
%token KW_return
%token KW_schema
%token KW_static
%token KW_table
%token KW_template
%token KW_trigger
%token KW_true
%token KW_type
%token KW_typedef
%token KW_typeset
%token KW_validate
%token KW_version
%token KW_view
%token KW_virtual
%token KW_void
%token KW_write

    /* Parse tree nodes */
%token PT_ASTLIST
%token PT_PARSE
%token PT_SOURCE
%token PT_VERSION_1_0
%token PT_VERSION_2
%token PT_SCHEMA_1_0
%token PT_INCLUDE
%token PT_TYPEDEF
%token PT_FQN
%token PT_IDENT
%token PT_PHYSIDENT
%token PT_UINT
%token PT_TYPESET
%token PT_TYPESETDEF
%token PT_FORMAT
%token PT_CONST
%token PT_ALIAS
%token PT_EXTERN
%token PT_FUNCTION
%token PT_UNTYPED
%token PT_ROWLENGTH
%token PT_FUNCDECL
%token PT_EMPTY
%token PT_SCHEMASIG
%token PT_SCHEMAFORMAL
%token PT_RETURNTYPE
%token PT_FACTSIG
%token PT_FUNCSIG
%token PT_FUNCPARAMS
%token PT_FORMALPARAM
%token PT_ELLIPSIS
%token PT_FUNCPROLOGUE
%token PT_RETURN
%token PT_PRODSTMT
%token PT_PRODTRIGGER
%token PT_SCHEMA
%token PT_VALIDATE
%token PT_PHYSICAL
%token PT_PHYSPROLOGUE
%token PT_PHYSSTMT
%token PT_PHYSBODYSTMT
%token PT_TABLE
%token PT_TABLEPARENTS
%token PT_TABLEBODY
%token PT_FUNCEXPR
%token PT_FACTPARMS
%token PT_COLUMN
%token PT_COLUMNEXPR
%token PT_COLDECL
%token PT_TYPEDCOL
%token PT_COLSTMT
%token PT_DFLTVIEW
%token PT_PHYSMBR
%token PT_PHYSCOL
%token PT_PHYSCOLDEF
%token PT_COLSCHEMAPARMS
%token PT_COLSCHEMAPARAM
%token PT_COLUNTYPED
%token PT_DATABASE
%token PT_TYPEEXPR
%token PT_DBBODY
%token PT_DBDAD
%token PT_DBMEMBER
%token PT_TBLMEMBER
%token PT_NOHEADER
%token PT_CASTEXPR
%token PT_CONSTVECT
%token PT_NEGATE
%token PT_UNARYPLUS
%token PT_VERSNAME
%token PT_ARRAY
%token PT_AT
%token PT_PHYSENCEXPR
%token PT_PHYSENCREF
%token PT_TYPEDCOLEXPR

 /* !!! Keep token declarations above in synch with schema-grammar.y */

%start parse

%type <node> source schema_1 schema_decls schema_decl schema_2 typedef new_type_names
%type <node> typeset typeset_spec typespec dim fmtdef const alias function func_decl
%type <node> schema_sig_opt return_type prologue formals_list
%type <node> schema_formals schema_formal type_expr formals formal
%type <node> script_stmts script_stmt extern_function script script_decl validate
%type <node> script_prologue physical phys_prologue phys_body phys_body_stmt
%type <node> phys_return_type table parents_opt tbl_body tbl_stmts tbl_stmt
%type <node> tbl_parents production column_decl col_modifiers_opt col_modifiers
%type <node> col_modifier col_decl col_ident col_body col_stmt typed_col column_expr
%type <node> factory_parms factory_parms_opt schema_parm schema_parms arrayspec
%type <node> phys_enc_ref col_body_opt database opt_dbdad dbbody db_members
%type <node> db_member template_opt

%type <fqn> fqn qualnames fqn_opt_vers ident fqn_vers

%type <expr> expr cond_expr cond_chain uint_expr

%type <paramSig> param_sig param_signature fact_sig

%type <boolean> vararg

%type <tok> END_SOURCE version_1 PT_VERSION_1_0 PT_VERSION_2 PT_SCHEMA_1_0 FLOAT version_2
%type <tok> PT_TYPEDEF PT_IDENT IDENTIFIER_1_0 DECIMAL PT_ASTLIST PT_ARRAY PT_TYPESET
%type <tok> PT_FORMAT PT_CONST PT_UINT PT_ALIAS PT_EMPTY PT_ELLIPSIS PT_RETURN
%type <tok> VERSION PT_UNTYPED PT_ROWLENGTH PT_FUNCDECL PT_FUNCPARAMS PT_FORMALPARAM
%type <tok> PT_VALIDATE PT_PHYSICAL PT_PHYSSTMT PT_TABLE PT_COLUMN PT_COLUMNEXPR
%type <tok> KW_default KW_extern KW_readonly PHYSICAL_IDENTIFIER_1_0 HEX OCTAL PT_COLSTMT
%type <tok> KW_read KW_validate KW_limit PT_SCHEMAFORMAL PT_PRODSTMT PT_PRODTRIGGER
%type <tok> PT_NOHEADER KW_decode KW_encode KW___row_length PT_COLDECL PT_TYPEDCOL PT_TYPEEXPR
%type <tok> PT_PHYSENCEXPR PT_PHYSENCREF KW_column PT_TYPEDCOLEXPR PT_DATABASE PT_DBBODY
%type <tok> KW_template KW_database PT_DBMEMBER PT_TBLMEMBER

%%

parse
    : PT_PARSE '(' END_SOURCE ')'                   { p_ast = new AST ( $3 ); }
    | PT_PARSE '(' source END_SOURCE ')'            { p_ast = $3; }
    ;

source
    : PT_SOURCE '(' schema_1 ')'                    { $$ = $3; }
    | PT_SOURCE '(' version_1 schema_1 ')'          { $$ = $4; }
    | PT_SOURCE '(' version_2 schema_2 ')'          { $$ = $4; dynamic_cast < AST_Schema* > ( $$ ) -> SetVersion ( $3 -> GetValue () ); }
    ;

version_1
    : PT_VERSION_1_0 '(' KW_version VERS_1_0 ';' ')'    { $$ = $1; }
    ;

version_2
    : PT_VERSION_2 '(' KW_version FLOAT ';' ')'     { $$ = $4; }
    ;

schema_1
    : PT_SCHEMA_1_0 '(' schema_decls ')'            { $$ = new AST_Schema ( $1, $3 ); }
    ;

 schema_2:
    %empty  { $$ = new AST_Schema (); }             /*TBD*/
    ;


schema_decls
    : schema_decl                                   { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | schema_decls schema_decl                      { $$ = $1; $$ -> AddNode ( $2 ); }
    ;

/* declarations */

schema_decl
    : typedef           { $$ = $1; }
    | typeset           { $$ = $1; }
    | fmtdef            { $$ = $1; }
    | const             { $$ = $1; }
    | alias             { $$ = $1; }
    | function          { $$ = $1; }
    | extern_function   { $$ = $1; }
    | script            { $$ = $1; }
    | validate          { $$ = $1; }
    | physical          { $$ = $1; }
    | table             { $$ = $1; }
    | database          { $$ = $1; }
    /*TBD*/
    | ';'               { $$ = new AST (); }
    ;

typedef
    : PT_TYPEDEF '(' KW_typedef fqn PT_ASTLIST '(' new_type_names ')' ';' ')'
                                            { $$ = p_builder . TypeDef ( $1, $4, $7 ); }
    ;

new_type_names
    : typespec                         { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | new_type_names ',' typespec      { $$ = $1; $$ -> AddNode ( $3 ); }
    ;

typespec
    : fqn       { $$ = $1; }
    | arrayspec { $$ = $1; }
    ;

arrayspec
    : PT_ARRAY '(' fqn '[' dim ']' ')'  { $$ = new AST ( $1, $3, $5 ); }
    ;

dim
    : expr   { $$ = $1; }
    | '*'    { $$ = new AST ( PT_EMPTY ); }
    ;

typeset
    :  PT_TYPESET '(' KW_typeset fqn PT_TYPESETDEF '(' '{' PT_ASTLIST '(' typeset_spec ')' '}' ')' ';' ')'
                { $$ = p_builder . TypeSet ( $1, $4, $10 ); }
    ;

typeset_spec
    : typespec                      { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | typeset_spec ',' typespec     { $$ = $1; $$ -> AddNode ( $3 ); }
    ;

fmtdef
    : PT_FORMAT '(' KW_fmtdef fqn ';' ')'       { $$ = p_builder . FmtDef ( $1, $4, 0 ); }
    | PT_FORMAT '(' KW_fmtdef fqn fqn ';' ')'   { $$ = p_builder . FmtDef ( $1, $5, $4 ); } /* note the flipped order */
    ;

const
    : PT_CONST '(' KW_const typespec fqn '=' expr ';' ')'        { $$ = p_builder . ConstDef ( $1, $4, $5, $7 ); }
    ;

alias
    : PT_ALIAS '(' KW_alias fqn fqn ';' ')'  { $$ = p_builder . AliasDef ( $1, $4, $5 ); }
    ;

function
    : PT_FUNCTION '(' KW_function func_decl ')' { $$ = $4; }
    ;

func_decl
    : PT_UNTYPED '(' KW___untyped fqn '(' ')' ')'      { $$ = p_builder . UntypedFunctionDecl ( $1, $4 ); }
    | PT_ROWLENGTH '(' KW___row_length fqn '(' ')' ')' { $$ = p_builder . RowlenFunctionDecl ( $1, $4 ); }
    | PT_FUNCDECL '(' schema_sig_opt return_type fqn_opt_vers fact_sig param_sig prologue ')'
                { $$ = p_builder . FunctionDecl ( $1, false, $3, $4, $5, $6, $7, $8 ); }
    ;

schema_sig_opt
    : PT_EMPTY                                                          { $$ = new AST ( $1 ); }
    | PT_SCHEMASIG '(' '<' PT_ASTLIST '(' schema_formals ')' '>' ')'    { $$ = $6; }
    ;

schema_formals
    : schema_formal                     { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | schema_formals ',' schema_formal  { $$ = $1; $$ -> AddNode ( $3 ); }
    ;

schema_formal
    : PT_SCHEMAFORMAL '(' KW_type ident ')'     { $$ = new AST ( $1, $4 ); }
    | PT_SCHEMAFORMAL '(' type_expr ident ')'   { $$ = new AST ( $1, $3, $4 ); }
    ;

return_type
    : PT_RETURNTYPE '(' KW_void ')'     { $$ = new AST (); }
    | PT_RETURNTYPE '(' type_expr ')'   { $$ = $3; }
    ;

fact_sig
    : PT_EMPTY                                   { $$ = new AST_ParamSig ( $1, 0, 0, false); }
    | PT_FACTSIG '(' '<' param_signature '>' ')' { $$ = $4; }
    ;

param_sig
    :  PT_FUNCSIG '(' '(' param_signature ')' ')' { $$ = $4; }
    ;

param_signature
    : PT_EMPTY                                                          { $$ = new AST_ParamSig ( $1, 0, 0, false); }
    | PT_FUNCPARAMS '(' formals_list vararg ')'                         { $$ = new AST_ParamSig ( $1, $3, 0, $4 ); }
    | PT_FUNCPARAMS '(' '*' formals_list vararg ')'                     { $$ = new AST_ParamSig ( $1, 0, $4, $5 ); }
    | PT_FUNCPARAMS '(' formals_list '*' formals_list vararg ')'        { $$ = new AST_ParamSig ( $1, $3, $5, $6 ); }
    | PT_FUNCPARAMS '(' formals_list ',' '*' formals_list vararg ')'    { $$ = new AST_ParamSig ( $1, $3, $6, $7 ); }
    ;

formals_list
    : PT_ASTLIST '(' formals ')'    { $$ = $3; }
    ;

formals
    : formal                { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | formals ',' formal    { $$ = $1; $$ -> AddNode ( $3 ); }
    ;

formal
    : PT_FORMALPARAM '(' typespec IDENTIFIER_1_0 ')'            { $$ = new AST_Formal ( $1, $3, $4, false ); }
    | PT_FORMALPARAM '(' KW_control typespec IDENTIFIER_1_0 ')' { $$ = new AST_Formal ( $1, $4, $5, true ); }
    ;

vararg
    : PT_EMPTY                          { $$ = false; }
    | PT_ELLIPSIS '(' ',' ELLIPSIS ')'  { $$ = true; }
    ;

prologue
    : PT_FUNCPROLOGUE '(' ';' ')'           { $$ = new AST ( PT_EMPTY ); }
    | PT_FUNCPROLOGUE '(' '=' fqn ';' ')'   { $$ = $4; }
    | script_prologue
    ;

script_prologue
    : PT_FUNCPROLOGUE '(' '{' PT_ASTLIST '(' script_stmts ')' '}' ')'   { $$ = $6; }
    ;

script_stmts
    : script_stmt               { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | script_stmts script_stmt  { $$ = $1; $$ -> AddNode ( $2 ); }
    ;

script_stmt
    : PT_RETURN '(' KW_return cond_expr ';' ')'     { $$ = new AST ( $1, $4 ); }
    | production                                    { $$ = $1; }
    ;

production
    : PT_PRODSTMT '(' type_expr ident '=' cond_expr ';' ')'        { $$ = new AST ( $1, $3, $4, $6 ); }
    | PT_PRODTRIGGER '(' KW_trigger ident '=' cond_expr ';' ')'    { $$ = new AST ( $1, $4, $6 ); }
    ;

extern_function
    : PT_EXTERN '(' KW_extern function ')'    { $$ = $4; }

script
    : PT_SCHEMA '(' KW_schema script_decl ')'             { $$ = $4; }
    | PT_SCHEMA '(' KW_schema KW_function script_decl ')' { $$ = $5; }
    ;

script_decl
    : PT_FUNCDECL '(' schema_sig_opt return_type fqn_opt_vers fact_sig param_sig script_prologue ')'
                { $$ = p_builder . FunctionDecl ( $1, true, $3, $4, $5, $6, $7, $8 ); }
    ;

validate
    : PT_VALIDATE '(' KW_validate
        PT_FUNCTION '(' KW_function
            PT_FUNCDECL '(' schema_sig_opt return_type fqn_opt_vers fact_sig param_sig prologue ')'
        ')'
      ')'
        { $$ = p_builder . FunctionDecl ( $1, false, $9, $10, $11, $12, $13, $14 ); }
    ;

physical
    : PT_PHYSICAL '(' KW_physical schema_sig_opt phys_return_type fqn_vers fact_sig phys_prologue ')'
            { $$ = p_builder . PhysicalDecl ( $1, $4, $5, $6, $7, $8 ); }
    ;

phys_return_type
    : return_type                                       { $$ = $1; }
    | PT_NOHEADER '(' KW___no_header return_type ')'    { $$ = new AST ( $1, $4 ); }

phys_prologue
    : PT_PHYSPROLOGUE '(' '=' PT_PHYSSTMT '(' '{' PT_ASTLIST '(' script_stmts ')' '}' ')' ')'
                { $$ = new AST ( $4, $9 ); }
    | PT_PHYSPROLOGUE '(' '{' PT_ASTLIST '(' phys_body ')' '}' ')'
                { $$ = $6; }
    ;

phys_body
    : phys_body_stmt            { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | phys_body phys_body_stmt  { $$ = $1; $$ -> AddNode ( $2 ); }
    ;

phys_body_stmt
    : PT_PHYSBODYSTMT '(' ';' ')'
        { $$ = new AST ( PT_EMPTY ); }
    | PT_PHYSBODYSTMT '(' KW_decode PT_PHYSSTMT '(' '{' PT_ASTLIST '(' script_stmts ')' '}' ')' ')'
        { $$ = new AST ( $3, $9 ) ; }
    | PT_PHYSBODYSTMT '(' KW_encode PT_PHYSSTMT '(' '{' PT_ASTLIST '(' script_stmts ')' '}' ')' ')'
        { $$ = new AST ( $3, $9 ); }
    | PT_PHYSBODYSTMT '(' KW___row_length '=' fqn '(' ')' ')'
        { $$ = new AST ( $3, $5 ); }
    ;

/* tables */

table
    : PT_TABLE '(' KW_table fqn_vers parents_opt PT_TABLEBODY '(' '{' tbl_body '}' ')' ')'
                { $$ = p_builder . TableDef ( $1, $4, $5, $9 ); }
    ;

parents_opt
    : PT_EMPTY                                                      { $$ = new AST ( $1 ); }
    | PT_TABLEPARENTS '(' '=' PT_ASTLIST '(' tbl_parents ')' ')'    { $$ = $6; }
    ;

tbl_parents
    : fqn_opt_vers                  { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | tbl_parents ',' fqn_opt_vers  { $$ = $1; $$ -> AddNode ( $3 ); }
    ;

tbl_body
    : %empty                        { $$ = new AST ( PT_EMPTY ); }
    | PT_ASTLIST '(' tbl_stmts ')'  { $$ = $3; }
    ;

tbl_stmts
    : tbl_stmt              { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | tbl_stmts tbl_stmt    { $$ = $1; $$ -> AddNode ( $2 ); }
    ;

tbl_stmt
    : production    { $$ = $1; }
    | column_decl   { $$ = $1; }
    | column_expr   { $$ = $1; }
    ;

column_decl
    : PT_COLUMN '(' col_modifiers_opt col_decl ')' { $$ = new AST ( $1, $3, $4 ); }
    ;

column_expr
    : PT_COLUMNEXPR '(' KW_column KW_limit '=' expr ';' ')'
        { $$ = new AST ( $1, $6 ); }
    | PT_COLUMNEXPR '(' KW_column KW_default KW_limit '=' expr ';' ')'
        { $$ = new AST ( $1, $7 ); }
    ;

col_modifiers_opt
    : KW_column                                     { $$ = new AST ( $1 ); }
    | PT_ASTLIST '(' col_modifiers KW_column ')'    { $$ = $3; }
    ;

col_modifiers
    : col_modifier                  { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | col_modifiers col_modifier    { $$ = $1; $$ -> AddNode ( $2 ); }
    ;

col_modifier
    : KW_default    { $$ = new AST ( $1 ); }
    | KW_extern     { $$ = new AST ( $1 ); }
    | KW_readonly   { $$ = new AST ( $1 ); }
    ;

col_decl
    : PT_COLDECL '(' typespec typed_col ')'         { $$ = new AST ( $1, $3, $4 ); }
    | PT_COLDECL '(' phys_enc_ref typed_col ')'    { $$ = new AST ( $1, $3, $4 ); }
    /*TBD*/
    ;

phys_enc_ref
    : PT_PHYSENCREF '(' '<' PT_ASTLIST '(' schema_parms ')' '>' fqn_opt_vers factory_parms_opt ')' { $$ = new AST ( $1, $6, $9, $10 ); }
    | PT_PHYSENCREF '(' fqn_vers factory_parms_opt ')'                          { $$ = new AST ( $1, $3, $4 ); }
    | PT_PHYSENCREF '(' fqn '<' factory_parms '>' ')'                           { $$ = new AST ( $1, $3, $5 ); }
    ;

typed_col
    : PT_TYPEDCOL '(' col_ident '{' col_body_opt '}' ')'
            {  $$ = new AST ( $1, $3, $5 ); }
    | PT_TYPEDCOLEXPR '(' col_ident '=' cond_expr ';' ')'
            {  $$ = new AST ( $1, $3, $5 ); }
    | PT_TYPEDCOL '(' col_ident ';' ')'
            {  $$ = new AST ( $1, $3 ); }
    ;

col_ident
    : ident                     { $$ = $1; }
    | PHYSICAL_IDENTIFIER_1_0   { $$ = new AST ( $1 ); }
    ;

col_body_opt
    : PT_EMPTY                      { $$ = new AST (); }
    | PT_ASTLIST '(' col_body ')'   { $$ = $3; }
    ;

col_body
    : col_stmt          { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | col_body col_stmt { $$ = $1; $$ -> AddNode ( $2 ); }
    ;

col_stmt
    : ';'                                           { $$ = new AST (); }
    | PT_COLSTMT '(' KW_read '=' cond_expr ';' ')'      { $$ = new AST ( $1, new AST ( $3 ), $5 ); }
    | PT_COLSTMT '(' KW_validate '=' cond_expr ';' ')'  { $$ = new AST ( $1, new AST ( $3 ), $5 ); }
    | PT_COLSTMT '(' KW_limit '=' uint_expr ';' ')'     { $$ = new AST ( $1, new AST ( $3 ), $5 ); }
    ;

/* expressions */

expr
    : uint_expr                 { $$ = $1; }
    | fqn                       { $$ = new AST_Expr ( $1 ); }
    | '@'                       { $$ = new AST_Expr ( PT_AT ); }
    | PHYSICAL_IDENTIFIER_1_0   { $$ = new AST_Expr ( $1 ); }
    /*| TBD */
    ;

cond_expr
    : PT_ASTLIST '(' cond_chain ')' { $$ = $3; }
    ;

cond_chain
    : expr                   { $$ = new AST_Expr ( $1 ); }
    | cond_chain '|' expr    { $$ = $1; $$ -> AddNode ( $3 ); }
    ;

type_expr
    : typespec                          { $$ = $1; }
    | PT_TYPEEXPR '(' fqn '/' fqn ')'   { $$ = new AST ( $1, $3, $5 ); }
    ;

uint_expr
    : PT_UINT '(' DECIMAL ')'   { $$ = new AST_Expr ( $1 ); $$ -> AddNode ( $3 ); }
    | PT_UINT '(' HEX ')'       { $$ = new AST_Expr ( $1 ); $$ -> AddNode ( $3 ); }
    | PT_UINT '(' OCTAL ')'     { $$ = new AST_Expr ( $1 ); $$ -> AddNode ( $3 ); }
    ;

/*
phys_enc_expr
    : PT_PHYSENCEXPR '(' '<' PT_ASTLIST '(' schema_parms ')' '>' fqn_opt_vers factory_parms_opt ')'
        { $$ = new AST_Expr ( $1 ); $$ -> AddNode ( $6 ); $$ -> AddNode ( $9 ); $$ -> AddNode ( $10 ); }
    ;
*/

schema_parms
    : schema_parm                   { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | schema_parms ',' schema_parm  { $$ = $1; $$ -> AddNode ( $3 ); }
    ;

schema_parm
    : fqn           { $$ = $1; }
    | arrayspec     { $$ = $1; }
    | uint_expr     { $$ = $1; }
    ;

factory_parms_opt
    : PT_EMPTY                                                          { $$ = new AST ( $1 ); }
    | PT_FACTPARMS '(' '<' PT_ASTLIST '(' factory_parms ')' '>' ')'     { $$ = $6; }
    ;

factory_parms
    : expr                      { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | factory_parms ',' expr    { $$ = $1; $$ -> AddNode ( $3 ); }
    ;

/* database */
database
    : PT_DATABASE '(' KW_database fqn_vers opt_dbdad dbbody ')' { $$ = p_builder . DatabaseDef ( $1, $4, $5, $6 ); }
    ;

opt_dbdad
    : PT_EMPTY                             { $$ = new AST ( $1 ); }
    | PT_DBDAD '(' '=' fqn_opt_vers ')'    { $$ = $4; }

dbbody
    : PT_DBBODY '(' '{' PT_ASTLIST '(' db_members ')' '}' ')'   { $$ = $6; }
    | PT_DBBODY '(' '{' '}' ')'                                 { $$ = new AST ( $1 ); }
    ;

db_members
    : db_member               { $$ = new AST (); $$ -> AddNode ( $1 ); }
    | ';'                       { $$ = new AST (); }
    | db_members db_member    { $$ = $1; $$ -> AddNode ( $2 ); }
    | db_members ';'            { $$ = $1; }
    ;

db_member
    : PT_DBMEMBER '(' template_opt KW_database fqn_opt_vers ident ';' ')'
        { $$ = new AST ( $1, $3, $5, $6 ); }
    | PT_TBLMEMBER '(' template_opt KW_table fqn_opt_vers ident ';' ')'
        { $$ = new AST ( $1, $3, $5, $6 ); }
    ;

template_opt
    : PT_EMPTY      { $$ = new AST ( $1 ); }
    | KW_template   { $$ = new AST ( $1 ); }
    ;

/* commonly used productions */

fqn
    : PT_FQN '(' qualnames ')'  { $$ = $3; }
    ;

qualnames
    : PT_IDENT '(' IDENTIFIER_1_0 ')'                   { $$ = new AST_FQN ( $1 ); $$ -> AddNode ( $3 ); }
    | qualnames ':' PT_IDENT  '(' IDENTIFIER_1_0 ')'    { $$ = $1; $$ -> AddNode ( $5 ); }
    ;

ident
    : PT_IDENT '(' IDENTIFIER_1_0 ')'   { $$ = new AST_FQN ( $1 ); $$ -> AddNode ( $3 ); }
    ;

fqn_opt_vers
    : fqn       { $$ = $1; }
    | fqn_vers  { $$ = $1; }
    ;

fqn_vers
    : PT_VERSNAME '(' fqn VERSION ')'   { $$ = $3; $$ -> SetVersion ( $4 -> GetValue () ); }
    ;

