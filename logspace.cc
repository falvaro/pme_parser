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

#include "logspace.h"

LogSpace::LogSpace(CYKcell *c, int nr, int dx, int dy) {
  //List length
  N=nr;
  //Reference symbol dimensions
  RX = dx;
  RY = dy;

  //Create a new vector to store the regions
  data = new CYKcell*[N];
  int i=0;
  for(CYKcell *r=c; r; r=r->next)
    data[i++] = r;

  //Sort regions according to x-coordinate
  quicksort(data, 0, N-1);
}

LogSpace::~LogSpace() {
  delete[] data;
}

void LogSpace::getH(CYKcell *c, list<CYKcell*> *set) {
  int sx, sy, ss, st;

  //Define search region
  sx = c->s - RX*0.75;  // (sx,sy)------
  ss = c->s + RX*3;     //  ------------
  sy = c->y - RY/2;     //  ------------
  st = c->t + RY/2;     //  ------(ss,st)

  bsearch(sx, sy, ss, st, set);
}

void LogSpace::getV(CYKcell *c, list<CYKcell*> *set) {
  int sx, sy, ss, st;

  //Define search region
  sx = c->x - RX;    // (sx,sy)------
  ss = c->s + RX;    //  ------------
  sy = c->t - RY/4;  //  ------------
  st = c->t + RY*3;  //  ------(ss,st)

  bsearchStv(sx, sy, ss, st, set, false);
}

void LogSpace::getU(CYKcell *c, list<CYKcell*> *set) {
  int sx, sy, ss, st;

  //Define search region
  sx = c->x - RX;    // (sx,sy)------
  ss = c->s + RX;    //  ------------
  sy = c->y - RY*3;  //  ------------
  st = c->y + RY/4;  //  ------(ss,st)

  bsearchStv(sx, sy, ss, st, set, true);
}

void LogSpace::getI(CYKcell *c, list<CYKcell*> *set) {
  int sx, sy, ss, st;

  //Define search region
  sx = c->x + 1;  // (sx,sy)------
  ss = c->s + RX; //  ------------
  sy = c->y + 1;  //  ------------
  st = c->t + RY; //  ------(ss,st)

  bsearch(sx, sy, ss, st, set);
}


void LogSpace::bsearch(int sx, int sy, int ss, int st, list<CYKcell*> *set) {
  //Binary search in O(logN) (key=sx)
  int i,j;
  for(i=0, j=N; i<j; ) {
    int m=(i+j)/2;

    if( sx <= data[m]->x )
      j=m;
    else
      i=m+1;
  }

  //Return compatible regions
  while( i<N && data[i]->x <= ss ) {
    if( data[i]->y <= st && data[i]->t >= sy )
      set->push_back(data[i]);
    i++;
  }
}


void LogSpace::bsearchStv(int sx, int sy, int ss, int st, list<CYKcell*> *set, bool U_V) {
  //Version for vertical relations where some overlaps (up or down) are avoided

  //Binary search in O(logN) (key=sx)
  int i,j;
  for(i=0, j=N; i<j; ) {
    int m=(i+j)/2;

    if( sx <= data[m]->x )
      j=m;
    else
      i=m+1;
  }

  //Return compatible regions
  if( U_V ) { //Direction 'Up' (U)
    while( i<N && data[i]->x <= ss ) {
      if( data[i]->t <= st && data[i]->t >= sy )
	set->push_back(data[i]);
      i++;
    }
  }
  else { //Direction 'Down' (V)
    while( i<N && data[i]->x <= ss ) {
      if( data[i]->y <= st && data[i]->y >= sy )
	set->push_back(data[i]);
      i++;
    }
  }
}


//Quicksort according to x-coordinate of region (x,y)-(s,t)
void LogSpace::quicksort(CYKcell **vec, int ini, int fin) {
  if( ini < fin ) {
    int piv = partition(vec, ini, fin);
    quicksort(vec, ini,   piv);
    quicksort(vec, piv+1, fin);
  }
}

//Partition algorithm
int LogSpace::partition(CYKcell **vec, int ini, int fin) {
  int piv = vec[ini]->x;
  int i=ini-1, j=fin+1;

  do{
    do{
      j--;
    }while(vec[j]->x > piv);
    do{
      i++;
    }while(vec[i]->x < piv);

    if( i<j ) {
      //Swap positions i, j
      CYKcell *aux = vec[i];
      vec[i] = vec[j];
      vec[j] = aux;
    }
  }while( i<j );

  return j;
}
