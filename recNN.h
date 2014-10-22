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

#ifndef _RECNN_
#define _RECNN_

#include <cstdio>
#include <string>
#include <map>
#include <vector>

class ProduccionT;

#include "production.h"

using namespace std;

class recNN{
  int **data;
  int *type;
  map<string,int> cl2key;
  vector<string> key2cl;

  int D; //Sample's dimensions
  int C; //Number of classes
  int N; //Number of samples

 public:
  recNN(FILE *bd, FILE *tp);
  ~recNN();

  void print();
  void classify(int *vec, int nbest, int *k, float *p);
  char *strClass(int c);
  int keyClass(char *str);
  int getNClasses();
  int symType(int k);
};


#endif
