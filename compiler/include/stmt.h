#ifndef _STMT_H_
#define _STMT_H_

#include <stdio.h>
#include "alist.h"
#include "baseAST.h"
#include "symbol.h"


extern bool printCppLineno;
extern bool printChplLineno;
extern bool inUserModule;
extern bool justStartedGeneratingFunction;

class Expr;
class DefExpr;

class Stmt : public BaseAST {
 public:
  Stmt* parentStmt;

  Stmt(astType_t astType = STMT);
  virtual ~Stmt() { }
  virtual void verify();
  COPY_DEF(Stmt);
  virtual void codegen(FILE* outfile);
  virtual void callReplaceChild(BaseAST* new_ast);
  virtual ASTContext getContext(void);
  virtual bool inTree();
  virtual void insertBefore(BaseAST* new_ast);
  virtual void insertAfter(BaseAST* new_ast);
};
#define forv_Stmt(_p, _v) forv_Vec(Stmt, _p, _v)


class ExprStmt : public Stmt {
 public:
  Expr* expr;

  ExprStmt(Expr* initExpr);
  virtual void verify();
  COPY_DEF(ExprStmt);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);

  virtual void print(FILE* outfile);
  virtual void codegen(FILE* outfile);
};


class ReturnStmt : public Stmt {
 public:
  Expr* expr;
  bool yield;
  ReturnStmt(Expr* initExpr = NULL, bool init_yield = false);
  ReturnStmt(Symbol* initExpr, bool init_yield = false);
  ReturnStmt(char* initExpr, bool init_yield = false);
  virtual void verify();
  COPY_DEF(ReturnStmt);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void print(FILE* outfile);
  void codegen(FILE* outfile);
  bool returnsVoid();
};


enum BlockTag {
  BLOCK_NORMAL = 0,
  BLOCK_SCOPELESS,
  BLOCK_WHILE_DO,
  BLOCK_DO_WHILE,
  BLOCK_FOR,
  BLOCK_FORALL,
  BLOCK_ORDERED_FORALL,
  BLOCK_PARAM_FOR,
  BLOCK_ATOMIC,
  BLOCK_COBEGIN,
  BLOCK_BEGIN
};


class BlockStmt : public Stmt {
 public:
  BlockTag blockTag;
  AList<Stmt>* body;
  SymScope* blkScope;
  LabelSymbol* pre_loop;
  LabelSymbol* post_loop;
  Expr* param_low; // for unrolling, low bound
  Expr* param_high; // for unrolling, high bound
  Expr* param_stride;
  Expr* param_index; // for unrolling, index variable

  BlockStmt::BlockStmt(AList<Stmt>* init_body = new AList<Stmt>(), 
                       BlockTag init_blockTag = BLOCK_NORMAL);
  BlockStmt::BlockStmt(Stmt* init_body,
                       BlockTag init_blockTag = BLOCK_NORMAL);
  BlockStmt::~BlockStmt();
  virtual void verify();
  COPY_DEF(BlockStmt);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void print(FILE* outfile);
  void codegen(FILE* outfile);

  void insertAtHead(BaseAST* ast);
  void insertAtTail(BaseAST* ast);
  void insertAtHead(AList<Stmt>* ast);
  void insertAtTail(AList<Stmt>* ast);

  bool isLoop(void);
};


class CondStmt : public Stmt {
 public:
  Expr* condExpr;
  BlockStmt* thenStmt;
  BlockStmt* elseStmt;

  CondStmt(Expr* iCondExpr, BaseAST* iThenStmt, BaseAST* iElseStmt = NULL);
  virtual void verify();
  COPY_DEF(CondStmt);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);

  void print(FILE* outfile);
  void codegen(FILE* outfile);
};


enum gotoType {
  goto_normal = 0,
  goto_break,
  goto_continue
};


class GotoStmt : public Stmt {
 public:
  Symbol* label;
  gotoType goto_type;

  GotoStmt(gotoType init_goto_type);
  GotoStmt(gotoType init_goto_type, char* init_label);
  GotoStmt(gotoType init_goto_type, Symbol* init_label);
  virtual void verify();
  COPY_DEF(GotoStmt);
  void print(FILE* outfile);
  void codegen(FILE* outfile);
};


#endif
