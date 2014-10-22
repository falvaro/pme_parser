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

#ifndef _CYKTABLE_
#define _CYKTABLE_

class ProductionB;
class ProductionT;

#include <cstdio>
#include <map>
#include "production.h"

using namespace std;

struct Symbol{
  int clase; //Symbol class (if no terminal production is -1)
  double pr; //Probability

  //Connected components
  int nc;
  bool *ccc;

  //Predecessors information (parsing tree)
  CYKcell *hi, *hd;
  ProductionB *prod;
  ProductionT *pt;

  //Baselines (relative to 'y') left (l) and right(r)
  int lbsup, rbsup;  //Superscript
  int lbhor, rbhor;  //Horizontal
  int lbsub, rbsub;  //Subscript

  Symbol(int c, double p, int ncc);
  ~Symbol();
};

struct CYKcell{
  //Region coordinates
  int x,y;
  int s,t;

  //Non-terminals that map the region
  int nnt;
  Symbol **ntsims;
  int nc; //Number of connected components

  //Next element in linked list (same level of the CYKtable)
  CYKcell *next;

  //Methods
  CYKcell(int n, int ncc);
  ~CYKcell();

  bool compatible(int ns, CYKcell *ot, int ons);
  void ccUnion(int ns, CYKcell *A, int nsa, CYKcell *B, int nsb);
  void print_tree(int n);
  void printSyms(int ns);
};


struct coo{
  int x,y,s,t;

  coo(int a, int b, int c, int d) {
    x=a; y=b; s=c; t=d;
  }
};

class CYKtable{
  CYKcell **T;
  map<coo,CYKcell*> *TS;
  int N, K;

 public:
  CYKtable(int n, int k);
  ~CYKtable();

  CYKcell *get(int n);
  int size(int n);
  void add(int n, CYKcell *celda);
};


#endif
