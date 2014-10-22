/*
* Copyright (C) 2011 Francisco Álvaro <falvaro@dsic.upv.es>.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or (at
* your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef _GRAMMAR_
#define _GRAMMAR_

class gParser;

#include <cstdio>
#include <string>
#include <map>
#include <list>
#include "production.h"
#include "recNN.h"
#include "sample.h"
#include "cyktable.h"
#include "gparser.h"

using namespace std;

class Grammar{
  map<string,int> nonTerminals;

  list<int> initsyms;
  list<ProductionB *> prodsH, prodsV, prodsVs, prodsIns, prodsSSE;
  list<ProductionT *> prodTerms;
  recNN *RecSims;

  int RX, RY;

  void initCYKterms(Sample *m, CYKtable *tcyk, int N, int K);
  void detRefSymbol(CYKtable *tcyk);
  void mergeCC(Sample *m, CYKtable *tcyk, int N);
  void print_spatialRel(CYKcell *cell, int n);
  const char *key2str(int k);
 public:
  Grammar(char *path);
  ~Grammar();

  void setSims(char *sims, char *info);
  void addInitSym(char *str);
  void addNoTerminal(char *str);
  void addTerminal(char *str, char *path);

  void addRuleH(float pr, char *S, char *A, char *B, char *out);
  void addRuleV(float pr, char *S, char *A, char *B, char *out, char *merge);
  void addRuleVs(float pr, char *S, char *A, char *B, char *out, char *merge);
  void addRuleSup(float pr, char *S, char *A, char *B, char *out);
  void addRuleSub(float pr, char *S, char *A, char *B, char *out);
  void addRuleSSE(float pr, char *S, char *A, char *B, char *out);
  void addRuleIns(float pr, char *S, char *A, char *B, char *out);

  CYKcell *fusion(ProductionB *pd, CYKcell *A, CYKcell *B, int N);
  void parse(Sample *m);
  //void print();
  void print_latex(CYKtable *T, int N);
  void viterbi(CYKcell *cell, int n);
};

#endif
