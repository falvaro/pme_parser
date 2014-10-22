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

#include "cyktable.h"

using namespace std;

//
//Symbol methods
//


Symbol::Symbol(int c, double p, int ncc) {
  clase = c;
  pr = p;
  hi = hd = NULL;
  prod = NULL;
  pt = NULL;
  lbsup = lbhor = lbsub = rbsup = rbhor = rbsub = 0;
  
  nc = ncc;
  ccc = new bool[nc];
  for(int i=0; i<nc; i++)
    ccc[i]= false;
}

Symbol::~Symbol() {
  delete[] ccc;
}

//
//CYKcell methods
//

CYKcell::CYKcell(int n, int ncc) {
  next = NULL;
  nnt = n;
  nc = ncc;

  ntsims = new Symbol*[nnt];
  for(int i=0; i<nnt; i++)
    ntsims[i] = NULL;
}

CYKcell::~CYKcell() {
  for(int i=0; i<nnt; i++)
    if( ntsims[i] )
      delete ntsims[i];

  delete[] ntsims;
}

bool CYKcell::compatible(int ns, CYKcell *ot, int ons) {
  for(int i=0; i<nc; i++)
    if( ntsims[ns]->ccc[i] && ot->ntsims[ons]->ccc[i] )
      return false;

  return true;
}

void CYKcell::ccUnion(int ns, CYKcell *A, int nsa, CYKcell *B, int nsb) {
  for(int i=0; i<nc; i++)
    if( A->ntsims[nsa]->ccc[i] || B->ntsims[nsb]->ccc[i] )
      ntsims[ns]->ccc[i] = true;
}

void CYKcell::print_tree(int n) {
  if( ntsims[n]->prod ) {
    int ps, pa, pb;
    ntsims[n]->prod->getData(&ps, &pa, &pb);

    printf("%d (", ps);
    ntsims[n]->hi->print_tree( pa );
    printf(")(");
    ntsims[n]->hd->print_tree( pb );
    printf(")");
  }
  else
    printf("%d (T%d)", n, ntsims[n]->clase);
}

void CYKcell::printSyms(int ns) {
  for(int i=0; i<nc; i++)
    if( ntsims[ns]->ccc[i] )
      printf(" %d", i);
}

bool operator < (const coo A, const coo B) {
  if( A.x < B.x )
    return true;
  if( A.x == B.x ) {
    if( A.y < B.y )
      return true;
    if( A.y == B.y ) {
      if( A.s < B.s )
	return true;
      if( A.s == B.s )
	if( A.t < B.t )
	  return true;
    }
  }
  return false;
}

//
//CYKtable methods
//

CYKtable::CYKtable(int n, int k) {
  N = n;
  K = k;

  T = new CYKcell *[N];
  for(int i=0; i<N; i++)
    T[i] = NULL;

  TS = new map<coo,CYKcell*>[N];
}

CYKtable::~CYKtable() {
  for(int i=0; i<N; i++)
    while( T[i] ) {
      CYKcell *aux = T[i]->next;
      delete T[i];
      T[i] = aux;
    }
  
  delete[] T;
  delete[] TS;
}

CYKcell *CYKtable::get(int n) {
  return T[n-1];
}

int CYKtable::size(int n) {
  return TS[n-1].size();
}

void CYKtable::add(int n, CYKcell *cell) {
  coo key(cell->x, cell->y, cell->s, cell->t);
  map<coo,CYKcell*>::iterator it=TS[n-1].find( key );

  if( it == TS[n-1].end() ) {
    //Link the element to level 'n'
    cell->next = T[n-1];
    T[n-1] = cell;
    TS[n-1][key] = cell;
  }
  else { //Avoid duplicates (maximizing probability)
    CYKcell *r = it->second;
    
    for(int i=0; i < r->nnt; i++) {
      if( r->ntsims[i] && cell->ntsims[i] ) {
	if( r->ntsims[i]->pr < cell->ntsims[i]->pr ) {
	  delete r->ntsims[i];
	  r->ntsims[i] = cell->ntsims[i];
	  cell->ntsims[i] = NULL;
	}
      }
      else
	if( cell->ntsims[i] && !r->ntsims[i] ) {
	  r->ntsims[i] = cell->ntsims[i];
	  //Mark as NULL to prevent that the destructor of 'cell' frees this Symbol
	  cell->ntsims[i] = NULL;
	}
    }
    delete cell;
  }

}

