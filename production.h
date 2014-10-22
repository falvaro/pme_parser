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

#ifndef _PRODUCTION_
#define _PRODUCTION_

class CYKcell;
class recNN;

#include "cyktable.h"
#include "recNN.h"

//Base class for binary productions of the grammar
class ProductionB{
 protected:
  int S;
  int A, B;
  float p;
  char *outStr;

  bool mergeSup;
  bool mergeHor;
  bool mergeSub;
 public:
  ProductionB(int s, int a, int b);
  ProductionB(int s, int a, int b, float pr, char *out);
  ~ProductionB();

  float getPrior();  
  void getData(int *s, int *a, int *b);
  float overlap(CYKcell *a, CYKcell *b);
  void printOut(CYKcell *cell);
  void printComps(CYKcell *cell, int sym);
  void setMerges(bool a, bool b, bool c);

  //Pure virtual functions
  virtual char type() = 0;
  virtual void print() = 0;
  virtual double prob(CYKcell *a, CYKcell *b, int rx, int ry) = 0;
  virtual void mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s) = 0;
};


//Production S -> A : B
class ProductionH : public ProductionB{

 public:
  ProductionH(int s, int a, int b);
  ProductionH(int s, int a, int b, float pr, char *out);
  
  void print();
  char type();
  double prob(CYKcell *a, CYKcell *b, int rx, int ry);
  void mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s);
};


//Production: S -> A / B
class ProductionV : public ProductionB{
  
 public:
  ProductionV(int s, int a, int b);
  ProductionV(int s, int a, int b, float pr, char *out);
  
  void print();
  char type();
  double prob(CYKcell *a, CYKcell *b, int rx, int ry);
  void mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s);
};


//Production: S -> A /u B
class ProductionU : public ProductionB{
  
 public:
  ProductionU(int s, int a, int b);
  ProductionU(int s, int a, int b, float pr, char *out);
  
  void print();
  char type();
  double prob(CYKcell *a, CYKcell *b, int rx, int ry);
  void mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s);
};


//Production: S -> A /s B
class ProductionVs : public ProductionB{
  
 public:
  ProductionVs(int s, int a, int b);
  ProductionVs(int s, int a, int b, float pr, char *out);
  
  void print();
  char type();
  double prob(CYKcell *a, CYKcell *b, int rx, int ry);
  void mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s);
};



//Production: S -> A sse B
class ProductionSSE : public ProductionB{
  
 public:
  ProductionSSE(int s, int a, int b);
  ProductionSSE(int s, int a, int b, float pr, char *out);
  
  void print();
  char type();
  double prob(CYKcell *a, CYKcell *b, int rx, int ry);
  void mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s);
};



//Production: S -> A ^ B
class ProductionSup : public ProductionB{
  
 public:
  ProductionSup(int s, int a, int b);
  ProductionSup(int s, int a, int b, float pr, char *out);
  
  void print();
  char type();
  double prob(CYKcell *a, CYKcell *b, int rx, int ry);
  void mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s);
};


//Production: S -> A _ B
class ProductionSub : public ProductionB{
  
 public:
  ProductionSub(int s, int a, int b);
  ProductionSub(int s, int a, int b, float pr, char *out);
  
  void print();
  char type();
  double prob(CYKcell *a, CYKcell *b, int rx, int ry);
  void mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s);
};


//Production: S -> A ins B
class ProductionIns : public ProductionB{
  
 public:
  ProductionIns(int s, int a, int b);
  ProductionIns(int s, int a, int b, float pr, char *out);
  
  void print();
  char type();
  double prob(CYKcell *a, CYKcell *b, int rx, int ry);
  void mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s);
};


//Production S -> term ( N classes )
class ProductionT{
  int S;
  bool *classes;
  char **texStr;
  float *probs;
  int N;

 public:
  ProductionT(int s, int nclasses);
  ~ProductionT();
  
  void setClass(int k, float pr, char *tex);
  bool getClass(int k);
  float getPrior(int k);
  char *getTeX(int k);
  int  getNoTerm();
  void print();
};

#endif
