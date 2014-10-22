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

#ifndef _LOGSPACE_
#define _LOGSPACE_

#include <cstdio>
#include <list>
#include "cyktable.h"

class LogSpace{
  int N;
  int RX, RY;
  CYKcell **data;

  void quicksort(CYKcell **vec, int ini, int fin);
  int partition(CYKcell **vec, int ini, int fin);
  void bsearch(int sx, int sy, int ss, int st, list<CYKcell*> *set);
  void bsearchStv(int sx, int sy, int ss, int st, list<CYKcell*> *set, bool U_V);

 public:
  LogSpace(CYKcell *c, int nr, int dx, int dy);
  ~LogSpace();

  void getH(CYKcell *c, list<CYKcell*> *set);
  void getV(CYKcell *c, list<CYKcell*> *set);
  void getU(CYKcell *c, list<CYKcell*> *set);
  void getI(CYKcell *c, list<CYKcell*> *set);
};

#endif
