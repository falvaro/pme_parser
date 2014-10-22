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

#include "mfset.h"
#include <cstdio>

using namespace std;

MFSET::MFSET(int dimX, int dimY) {
  rowWidth = dimX;
  nElem = nSets = dimX * dimY;
  mf = new int[nElem];

  for(int i=0; i<nElem; i++)
    mf[i] = -1;
}

MFSET::~MFSET() {
  delete[] mf;
}

int MFSET::get_nSets() {
  return nSets;
}

int MFSET::find(int x) {
  if( mf[x] < 0 )
    return x;

  int rx = x;
  while( mf[rx] >= 0) //Look for the representative
    rx = mf[rx];
  
  while (mf[x] != rx) { //Path compression
    int tmp = x;
    x = mf[x];
    mf[tmp] = rx;
  }

  return rx;
}

void MFSET::merge(int x, int y) {
  int rx = find(x);
  int ry = find(y);
  
  if( rx != ry ) {
    nSets--;
    if (mf[rx] == mf[ry]) { //height(x) == height(y)
      mf[ry] = rx;
      mf[rx]--; //Increase height(x)
    }
    else { //Different height
      if( mf[rx] < mf[ry] ) // height(x) > height(y)
	mf[ry] = rx;
      else
	mf[rx] = ry;
    }
  }
}

//Version in coordinates
int MFSET::find(int x, int y) {
  return find( y*rowWidth + x );
}

void MFSET::merge(int a_x, int a_y, int b_x, int b_y) {
  merge(a_y*rowWidth + a_x, b_y*rowWidth + b_x);
}
