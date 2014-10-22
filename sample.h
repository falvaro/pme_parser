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

#ifndef _SAMPLE_
#define _SAMPLE_

#include <cstdio>
#include <Magick++.h>
#include "mfset.h"
#include "cyktable.h"

using namespace std;
using namespace Magick;

//Connected component information
struct component{
  int rp; //key in MFSET
  //Minimum bounding box
  int x, y; //Upper left corner (x,y)
  int s, t; //Bottom right corner   (s,t)
};


class Sample{
  unsigned char **data;
  Image *img;
  int X, Y;
  MFSET *mfset;
  //Connected components
  component *comps;
  int NC;

  void getRegion(int *vec, int x, int y, int s, int t, int rp1, int rp2=-1);

public:
  Sample(char *str);
  ~Sample();

  int dimX();
  int dimY();
  int nComponents();
  unsigned char get(int x, int y);

  //One component
  void getRegion(int *vec, int nComp, int *as, int *cn, int *ds);
  void setRegion(CYKcell *c, int nComp);
  //Two components
  void getRegion(int *vec, int nComp, int otroRp, int *as, int *cn, int *ds);
  void setRegion(CYKcell *c, int nComp, int otroRp);

  int getCandidates(int nComp, int *cand, int dx, int dy);
  int rp2cmp(int rp);

  void print();
};

#endif
