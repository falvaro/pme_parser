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

#include <cstdio>
#include <cstring>
#include <cmath>
#include <cfloat>
#include "production.h"

#define OVERLAP 0.85

using namespace std;

//Auxiliary functions

inline int abs(int x) {
  return (x>0) ? x : -x;
}

inline int max(int x, int y) {
  return (x>y) ? x : y;
}

inline int min(int x, int y) {
  return (x<y) ? x : y;
}

int check(char *str, char *pat) {
  for(int i=0; str[i]; i++ ) 
    if( str[i] == pat[0] ) {
      int j=1;
      while( str[i+1] && pat[j] ) {
	if( str[i+j] != pat[j] )
	  break;
	j++;
      }
      if( !pat[j] )
	return i;
    }

  return -1;
}


//
//ProductionB methods
//

ProductionB::ProductionB(int s, int a, int b) {
  S = s;
  A = a;
  B = b;
  outStr = NULL;
}

ProductionB::ProductionB(int s, int a, int b, float pr, char *out) {
  S = s;
  A = a;
  B = b;
  p = pr > 0.0 ? log(pr) : -FLT_MAX;

  setMerges(false,false,false);

  outStr = new char[strlen(out)+1];
  strcpy(outStr, out);
}

ProductionB::~ProductionB() {
  if( outStr ) delete[] outStr;
}

float ProductionB::getPrior() {
  return p;
}

void ProductionB::getData(int *s, int *a, int *b) {
  if( s ) *s = S;
  if( a ) *a = A;
  if( b ) *b = B;
}


//Percentage of area of region A that overlaps with region B
float ProductionB::overlap(CYKcell *a, CYKcell *b) {
  int x = max(a->x, b->x);
  int y = max(a->y, b->y);
  int s = min(a->s, b->s);
  int t = min(a->t, b->t);
  
  if( s >= x && t >= y ) {
    int aOverl = (s-x+1)*(t-y+1);
    int aTotal = (a->s - a->x+1)*(a->t - a->y+1);

    return (float)aOverl/aTotal;
  }
  
  return 0.0;
}


void ProductionB::printOut(CYKcell *cell) {
  if( outStr ) {
    int pd1 = check(outStr, (char*)"$1");
    int pd2 = check(outStr, (char*)"$2");

    int i=0;
    if( pd2 >= 0 && pd1 >= 0 && pd2 < pd1 ) {
      while( outStr[i]!='$' || outStr[i+1] != '2') {
	putchar(outStr[i]);
	i++;
      }
      i+=2;
      
      if( cell->ntsims[S]->hd->ntsims[B]->clase < 0 )
	cell->ntsims[S]->hd->ntsims[B]->prod->printOut(cell->ntsims[S]->hd);
      else
	printf("%s", cell->ntsims[S]->hd->ntsims[B]->pt->getTeX(cell->ntsims[S]->hd->ntsims[B]->clase));

      while( outStr[i]!='$' || outStr[i+1] != '1') {
	putchar(outStr[i]);
	i++;
      }
      i+=2;

      if( cell->ntsims[S]->hi->ntsims[A]->clase < 0 )
	cell->ntsims[S]->hi->ntsims[A]->prod->printOut(cell->ntsims[S]->hi);
      else
	printf("%s", cell->ntsims[S]->hi->ntsims[A]->pt->getTeX(cell->ntsims[S]->hi->ntsims[A]->clase));
    }
    else {
      if( pd1 >= 0 ) {
	while( outStr[i]!='$' || outStr[i+1] != '1') {
	  putchar(outStr[i]);
	  i++;
	}
	i+=2;
	
	if( cell->ntsims[S]->hi->ntsims[A]->clase < 0 )
	  cell->ntsims[S]->hi->ntsims[A]->prod->printOut(cell->ntsims[S]->hi);
	else
	  printf("%s", cell->ntsims[S]->hi->ntsims[A]->pt->getTeX(cell->ntsims[S]->hi->ntsims[A]->clase));
      }
      if( pd2 >= 0 ) {
	while( outStr[i]!='$' || outStr[i+1] != '2') {
	  putchar(outStr[i]);
	  i++;
	}
	i+=2;
	
	if( cell->ntsims[S]->hd->ntsims[B]->clase < 0 )
	  cell->ntsims[S]->hd->ntsims[B]->prod->printOut(cell->ntsims[S]->hd);
	else
	  printf("%s", cell->ntsims[S]->hd->ntsims[B]->pt->getTeX(cell->ntsims[S]->hd->ntsims[B]->clase));
      }
    }
    while( outStr[i] ) {
      putchar(outStr[i]);
      i++;
    }
  }
}


void ProductionB::printComps(CYKcell *cell, int sym) {
  if( cell->ntsims[sym]->clase >= 0 ) {
    printf("#%d_%d_%d_%d %s\n", cell->x, cell->y, cell->s, cell->t, 
	cell->ntsims[sym]->pt->getTeX(cell->ntsims[sym]->clase));
  }
  else {
    cell->ntsims[sym]->hi->ntsims[A]->prod->printComps(cell->ntsims[S]->hi, A);
    cell->ntsims[sym]->hd->ntsims[B]->prod->printComps(cell->ntsims[S]->hd, B);
  }
}

void ProductionB::setMerges(bool a, bool b, bool c) {
  mergeSup = a;
  mergeHor = b;
  mergeSub = c;
}


//
//ProductionH methods
//

ProductionH::ProductionH(int s, int a, int b)
  : ProductionB(s, a, b)
{}

ProductionH::ProductionH(int s, int a, int b, float pr, char *out)
  : ProductionB(s, a, b, pr, out)
{}

void ProductionH::print() {
  printf("%d -> %d : %d\n", S, A, B);
}

char ProductionH::type() {
  return 'H';
}

void ProductionH::mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s) {
  //Left baseline
  s->ntsims[S]->lbsup = a->ntsims[A]->lbsup;
  s->ntsims[S]->lbhor = a->ntsims[A]->lbhor;
  s->ntsims[S]->lbsub = a->ntsims[A]->lbsub;
  //Right baseline
  s->ntsims[S]->rbsup = b->ntsims[B]->rbsup;
  s->ntsims[S]->rbhor = b->ntsims[B]->rbhor;
  s->ntsims[S]->rbsub = b->ntsims[B]->rbsub;
}

//Probability of horizontal arrangement between regions 'a' and 'b'
double ProductionH::prob(CYKcell *a, CYKcell *b, int rx, int ry) {
  if( overlap(a,b) > OVERLAP || overlap(b,a) > OVERLAP )
    return 0.0;

  if( b->x < (a->s - min(rx,a->s-a->x)/2) )
    return 0.0;

  int bh = b->ntsims[B]->lbhor;
  int ah = a->ntsims[A]->rbhor;
  int amy = a->t - a->y;
  float HR = max(ry,amy);
  if( bh < (ah - HR*0.7) || bh > (ah + HR*0.7) )
    return 0.0;
  
  int dx  = abs(b->x - a->s);
  float p1 = 1.0 - dx/(3.0*rx);
  float p2 = 1.0 - abs(bh - ah)/HR;
  
  if( p1 <= 0.0 || p2 <= 0.0 )
    return 0.0;

#ifdef VERBOSE
  fprintf(stderr, "p( (%d,%d)(%d,%d) H (%d,%d)(%d,%d) ) = %g\n", a->x,a->y,a->s,a->t,b->x,b->y,b->s,b->t, (p1+p2)/2);
#endif

  return (p1+p2)/2;
}



//
//ProductionV methods
//

ProductionV::ProductionV(int s, int a, int b)
  : ProductionB(s, a, b)
{}

ProductionV::ProductionV(int s, int a, int b, float pr, char *out)
  : ProductionB(s, a, b, pr, out)
{}

void ProductionV::print() {
  printf("%d -> %d / %d\n", S, A, B);
}

char ProductionV::type() {
  return 'V';
}

void ProductionV::mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s) {
  //Left baseline
  s->ntsims[S]->lbsup = mergeSup ? a->ntsims[A]->lbsup : b->ntsims[B]->lbsup;
  s->ntsims[S]->lbhor = mergeHor ? a->ntsims[A]->lbhor : b->ntsims[B]->lbhor;
  s->ntsims[S]->lbsub = mergeSub ? a->ntsims[A]->lbsub : b->ntsims[B]->lbsub;
  //Right baseline
  s->ntsims[S]->rbsup = mergeSup ? a->ntsims[A]->rbsup : b->ntsims[B]->rbsup;
  s->ntsims[S]->rbhor = mergeHor ? a->ntsims[A]->rbhor : b->ntsims[B]->rbhor;
  s->ntsims[S]->rbsub = mergeSub ? a->ntsims[A]->rbsub : b->ntsims[B]->rbsub;
}


//Probability of vertical arrangement between regions 'a' and 'b'
double ProductionV::prob(CYKcell *a, CYKcell *b, int rx, int ry) {
  if( overlap(a,b) > OVERLAP || overlap(b,a) > OVERLAP )
    return 0.0;

  if( b->y < a->t )
    return 0.0;

  int amx = a->s - a->x;
  float WR = max(rx,amx)*0.9;
  int cb = b->x + (b->s - b->x)/2;
  if( cb < (a->x - WR*0.7) || cb > (a->s + WR*0.7) )
    return 0.0;

  int dy  = abs(b->y - a->t);
  float p1 = 1.0 - dy/(3.0*ry);
  float p2 = 1.0 - abs(cb-(a->x+amx/2))/WR;

  if( p1 <= 0.0 || p2 <= 0.0 )
    return 0.0;

#ifdef VERBOSE
  fprintf(stderr, "p( (%d,%d)(%d,%d) V (%d,%d)(%d,%d) ) = %g\n",
	  a->x,a->y,a->s,a->t,b->x,b->y,b->s,b->t, (p1+p2)/2);
#endif  

  return (p1+p2)/2;
}




//
//ProductionVs methods
//

ProductionVs::ProductionVs(int s, int a, int b)
  : ProductionB(s, a, b)
{}

ProductionVs::ProductionVs(int s, int a, int b, float pr, char *out)
  : ProductionB(s, a, b, pr, out)
{}

void ProductionVs::print() {
  printf("%d -> %d /s %d\n", S, A, B);
}

char ProductionVs::type() {
  return 'e';
}

void ProductionVs::mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s) {
  //Left baseline
  s->ntsims[S]->lbsup = mergeSup ? a->ntsims[A]->lbsup : b->ntsims[B]->lbsup;
  s->ntsims[S]->lbhor = mergeHor ? a->ntsims[A]->lbhor : b->ntsims[B]->lbhor;
  s->ntsims[S]->lbsub = mergeSub ? a->ntsims[A]->lbsub : b->ntsims[B]->lbsub;
  //Right baseline
  s->ntsims[S]->rbsup = mergeSup ? a->ntsims[A]->rbsup : b->ntsims[B]->rbsup;
  s->ntsims[S]->rbhor = mergeHor ? a->ntsims[A]->rbhor : b->ntsims[B]->rbhor;
  s->ntsims[S]->rbsub = mergeSub ? a->ntsims[A]->rbsub : b->ntsims[B]->rbsub;
}

//Probability of (strict) vertical arrangement between regions 'a' and 'b'
double ProductionVs::prob(CYKcell *a, CYKcell *b, int rx, int ry) {
  if( overlap(a,b) > OVERLAP || overlap(b,a) > OVERLAP )
    return 0.0;

  if( b->y < a->t )
    return 0.0;

  int dy  = abs(b->y - a->t);
  float p1 = 1.0 - dy/(3.0*ry);
  float p2 = 1.0 - (abs(a->x - b->x)+abs(a->s - b->s))/(3.0*rx);

  if( p1 <= 0.0 || p2 <= 0.0 )
    return 0.0;

#ifdef VERBOSE
  fprintf(stderr, "p( (%d,%d)(%d,%d) Ve (%d,%d)(%d,%d) ) = %g\n",
	  a->x,a->y,a->s,a->t,b->x,b->y,b->s,b->t, (p1+p2)/2);
#endif
  
  return (p1+p2)/2;
}


//
//ProductionSSE methods
//

ProductionSSE::ProductionSSE(int s, int a, int b)
  : ProductionB(s, a, b)
{}

ProductionSSE::ProductionSSE(int s, int a, int b, float pr, char *out)
  : ProductionB(s, a, b, pr, out)
{}

void ProductionSSE::print() {
  printf("%d -> %d sse %d\n", S, A, B);
}

char ProductionSSE::type() {
  return 'S';
}

void ProductionSSE::mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s) {
  //Left baseline
  s->ntsims[S]->lbsup = a->ntsims[A]->lbhor;
  s->ntsims[S]->lbhor = (a->ntsims[A]->lbhor+b->ntsims[B]->lbhor)/2;
  s->ntsims[S]->lbsub = b->ntsims[B]->lbhor;
  //Right baseline
  s->ntsims[S]->rbsup = a->ntsims[A]->rbhor;
  s->ntsims[S]->rbhor = (a->ntsims[A]->rbhor+b->ntsims[B]->rbhor)/2;
  s->ntsims[S]->rbsub = b->ntsims[B]->rbhor;
}

double ProductionSSE::prob(CYKcell *a, CYKcell *b, int rx, int ry) {
  if( overlap(a,b) > OVERLAP || overlap(b,a) > OVERLAP )
    return 0.0;

  if( b->y < a->t )
    return 0.0;

  float p1 = 1.0 - abs(b->y - a->t)/(3.0*ry);
  float p2 = 1.0 - abs(a->x - b->x)/(3.0*rx);

  if( p1 <= 0.0 || p2 <= 0.0 )
    return 0.0;

#ifdef VERBOSE
  fprintf(stderr, "p( (%d,%d)(%d,%d) SSE (%d,%d)(%d,%d) ) = %g\n",
	  a->x,a->y,a->s,a->t,b->x,b->y,b->s,b->t, (p1+p2)/2);
#endif
  
  return (p1+p2)/2;
}



//
//ProductionSup methods
//

ProductionSup::ProductionSup(int s, int a, int b)
  : ProductionB(s, a, b)
{}

ProductionSup::ProductionSup(int s, int a, int b, float pr, char *out)
  : ProductionB(s, a, b, pr, out)
{}

void ProductionSup::print() {
  printf("%d -> %d ^ %d\n", S, A, B);
}

char ProductionSup::type() {
  return 'P';
}

void ProductionSup::mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s) {
  //Left baseline
  s->ntsims[S]->lbsup = a->ntsims[A]->lbsup;
  s->ntsims[S]->lbhor = a->ntsims[A]->lbhor;
  s->ntsims[S]->lbsub = a->ntsims[A]->lbsub;
  //Right baseline
  s->ntsims[S]->rbsup = b->ntsims[B]->rbhor;
  s->ntsims[S]->rbhor = a->ntsims[A]->rbhor;
  s->ntsims[S]->rbsub = a->ntsims[A]->rbsub;
}

double ProductionSup::prob(CYKcell *a, CYKcell *b, int rx, int ry) {
  if( overlap(a,b) > OVERLAP || overlap(b,a) > OVERLAP )
    return 0.0;

  if( b->x < (a->s - min(rx,a->s-a->x)/2) )
    return 0.0;

  int bh = b->ntsims[B]->lbhor;
  int as = a->ntsims[A]->rbsup;
  float HR = max(ry, a->t - a->y);
  if( bh < (as - HR*0.7) || bh > (as + HR*0.7) )
    return 0.0;

  int dx  = abs(b->x - a->s);
  float p1 = 1.0 - dx/(3.0*rx);
  float p2 = 1.0 - abs(bh - as)/HR;

  if( p1 <= 0.0 || p2 <= 0.0 )
    return 0.0;

  float ha = a->ntsims[A]->rbsub - a->ntsims[A]->rbsup;
  float hb = b->ntsims[B]->lbsub - b->ntsims[B]->lbsup;
  if( ha <= 0.0 ) ha=0.1;
  if( hb <= 0.0 ) hb=0.1;

  float pr = (p1+p2)/2;
  if( ha/hb < 1.0 )
    pr *= (ha/hb)-0.05;

#ifdef VERBOSE
  fprintf(stderr, "p( (%d,%d)(%d,%d) Sup (%d,%d)(%d,%d) ) = %g\n", a->x,a->y,a->s,a->t,b->x,b->y,b->s,b->t, pr);
#endif  

  return pr;
}



//
//ProductionSub methods
//

ProductionSub::ProductionSub(int s, int a, int b)
  : ProductionB(s, a, b)
{}

ProductionSub::ProductionSub(int s, int a, int b, float pr, char *out)
  : ProductionB(s, a, b, pr, out)
{}

void ProductionSub::print() {
  printf("%d -> %d _ %d\n", S, A, B);
}

char ProductionSub::type() {
  return 'B';
}

void ProductionSub::mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s) {
  //Left baseline
  s->ntsims[S]->lbsup = a->ntsims[A]->lbsup;
  s->ntsims[S]->lbhor = a->ntsims[A]->lbhor;
  s->ntsims[S]->lbsub = a->ntsims[A]->lbsub;
  //Right baseline
  s->ntsims[S]->rbsup = a->ntsims[A]->rbsup;
  s->ntsims[S]->rbhor = a->ntsims[A]->rbhor;
  s->ntsims[S]->rbsub = b->ntsims[B]->rbhor;
}

double ProductionSub::prob(CYKcell *a, CYKcell *b, int rx, int ry) {
  if( overlap(a,b) > OVERLAP || overlap(b,a) > OVERLAP )
    return 0.0;

  if( b->x < (a->s - min(rx,a->s-a->x)/2) )
    return 0.0;
  
  int bh = b->ntsims[B]->lbhor;
  int as = a->ntsims[A]->rbsub;
  float HR = max(ry, a->t - a->y);
  if( bh < (as - HR*0.7) || bh > (as + HR*0.7) )
    return 0.0;
  
  int dx = abs(b->x - a->s);
  float p1 = 1.0 - dx/(3.0*rx);
  float p2 = 1.0 - abs(bh - as)/HR;

  if( p1 <= 0.0 || p2 <= 0.0 )
    return 0.0;
  
  float ha = a->ntsims[A]->rbsub - a->ntsims[A]->rbsup;
  float hb = b->ntsims[B]->lbsub - b->ntsims[B]->lbsup;
  if( ha <= 0.0 ) ha=0.1;
  if( hb <= 0.0 ) hb=0.1;

  float pr = (p1+p2)/2;
  if( ha/hb < 1.0 )
  pr *= (ha/hb)-0.05;

#ifdef VERBOSE
  fprintf(stderr, "p( (%d,%d)(%d,%d) Sub (%d,%d)(%d,%d) ) = %g\n", a->x,a->y,a->s,a->t,b->x,b->y,b->s,b->t, pr);
#endif  

  return (p1+p2)/2;
}





//
//ProductionIns methods
//

ProductionIns::ProductionIns(int s, int a, int b)
  : ProductionB(s, a, b)
{}

ProductionIns::ProductionIns(int s, int a, int b, float pr, char *out)
  : ProductionB(s, a, b, pr, out)
{}

void ProductionIns::print() {
  printf("%d -> %d /e %d\n", S, A, B);
}

char ProductionIns::type() {
  return 'I';
}

void ProductionIns::mergeRegions(CYKcell *a, CYKcell *b, CYKcell *s) {
  //Left baseline
  s->ntsims[S]->lbsup = a->ntsims[A]->lbsup;
  s->ntsims[S]->lbhor = a->ntsims[A]->lbhor;
  s->ntsims[S]->lbsub = a->ntsims[A]->lbsub;
  //Right baseline
  s->ntsims[S]->rbsup = b->ntsims[B]->rbsup;
  s->ntsims[S]->rbhor = b->ntsims[B]->rbhor;
  s->ntsims[S]->rbsub = b->ntsims[B]->rbsub;
}

double ProductionIns::prob(CYKcell *a, CYKcell *b, int rx, int ry) {
  if( overlap(b,a) < OVERLAP )
    return 0.0;

  if( b->x < a->x || b->y < a->y )
    return 0.0;

  int dy = abs(a->t - b->t);
  int dx = abs(a->s - b->s);

  float p = 1.0 - (float)(dx*dx + dy*dy)/(rx*rx + ry*ry);

#ifdef VERBOSE
  fprintf(stderr, "p( (%d,%d)(%d,%d) Ins (%d,%d)(%d,%d) ) = %g\n",
	  a->x,a->y,a->s,a->t,b->x,b->y,b->s,b->t, p);
#endif
  
  return p;
}






//
//ProductionT methods
//

ProductionT::ProductionT(int s, int nclasses) {
  S = s;
  N = nclasses;
  texStr = new char*[N];
  classes = new bool[N];
  probs = new float[N];
  for(int i=0; i<N; i++) {
    classes[i] = false;
    texStr[i] = NULL;
    probs[i] = 0;
  }
}

ProductionT::~ProductionT() {
  delete[] classes;
  delete[] texStr;
  delete[] probs;
}

void ProductionT::setClass(int k, float pr, char *tex) {
  classes[k] = true;
  if( texStr[k] )
    fprintf(stderr, "Warning: Terminal %d redefined with label '%s'\n", k, tex);
  else {
    texStr[k] = new char[strlen(tex)+1];
    strcpy(texStr[k], tex);
    probs[k] = pr > 0.0 ? log(pr) : -FLT_MAX;
  }
}

bool ProductionT::getClass(int k) {
  return classes[k];
}

char *ProductionT::getTeX(int k) {
  return texStr[k];
}

float ProductionT::getPrior(int k) {
  return probs[k];
}


int ProductionT::getNoTerm() {
  return S;
}

void ProductionT::print() {
  int nc=0;

  for(int i=0; i<N; i++)
    if( classes[i] )
      nc++;

  printf("%d -> [%d classes]\n", S, nc);
}
