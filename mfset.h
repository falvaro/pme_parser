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

#ifndef _MFSET_
#define _MFSET_

#include <cstdio>

using namespace std;

class MFSET{
  int *mf;
  int nElem, rowWidth;
  int nSets;

public:
  MFSET(int dimX, int dimY);
  ~MFSET();

  int get_nSets();
  int find(int x);
  void merge(int x, int y);

  //Version in coordinates
  int find(int x, int y);
  void merge(int a_x, int a_y, int b_x, int b_y);
};

#endif
