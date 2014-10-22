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
#include <cstdlib>
#include <cmath>
#include <vector>
#include <map>
#include <climits>
#include <cfloat>
#include "recNN.h"
#include "production.h"

recNN::recNN(FILE *bd, FILE *tp) {
  //Read number of samples
  fscanf(bd, "%d", &N); getc(bd);
  D=225; //15x15
  C=0;

  data = new int *[N];

  for(int i=0; i<N; i++) {
    data[i] = new int[D+1];

    char clase[256];
    fscanf(bd, "%s", clase);

    if( cl2key.find(clase) == cl2key.end() ) {
      cl2key[clase] = C;
      key2cl.push_back(clase);

      data[i][0] = C;
      C++;
    }
    else
      data[i][0] = cl2key[clase];

    for(int j=1; j<=D ; j++)
      fscanf(bd, "%d", &data[i][j]);

    getc(bd); //Skip the newline character
  }

  //Load information about symbol types
  type = new int[C];

  char clase[256], T=0, line[256];
  while( fgets(line, 256, tp) != NULL ) {
    for(int i=0; line[i] && line[i] != '\n'; i++) {
      clase[i] = line[i];
      if( line[i]==' ' ) {
	clase[i] = 0;
	T = line[i+1];
	break;
      }
    }

    if( cl2key.find(clase) == cl2key.end() )
      continue;

    if( T=='n' )       type[ cl2key[clase] ] = 0; //Normal
    else if( T=='a' )  type[ cl2key[clase] ] = 1; //Ascendant
    else if( T=='d' )  type[ cl2key[clase] ] = 2; //Descending
    else {
      fprintf(stderr, "recNN: Error loading symbol types\n");      
      exit(-1);
    }
  }
}

recNN::~recNN() {
  for(int i=0; i<N; i++)
    delete[] data[i];
  delete[] data;
  delete[] type;
}

void recNN::print() {
  printf("%d %d %d\n", N, D, C);
  for(int i=0; i<N; i++) {
    printf("%s ", (key2cl[data[i][0]]).c_str());
    for(int j=1; j<=D; j++)
      printf("%d", data[i][j]);
    printf("\n");
  }
}

void recNN::classify(int *vec, int nbest, int *k, float *p) {
  for(int i=0; i<nbest; i++) {
    p[i] = FLT_MAX;
    k[i] = -1;
  }

  for(int i=0; i < N; i++) {
    int dis=0;
    for(int j=0; j<D; j++)
      dis += (vec[j]-data[i][j+1])*(vec[j]-data[i][j+1]);
  
    if( dis < p[0] ) {
      int insp=0;
      bool insert=true;
      for(int q=0; q<nbest; q++)
	if( k[q] == data[i][0] ) {
	  if( dis >= p[q] )
	    insert=false;
	  else
	    insp=q;
	  break;
	}

      if( !insert ) continue;

      p[insp] = dis;
      k[insp] = data[i][0];

      //Reorder nbest list
      for(int q=insp+1; q<nbest; q++)
	if( p[q] > p[q-1] ) {
	  float aux = p[q];
	  p[q] = p[q-1];
	  p[q-1] = aux;
	  aux = k[q];
	  k[q] = k[q-1];
	  k[q-1] = aux;
	}
	else break;

    }
  }

  for(int i=0; i<nbest; i++)
    p[i] = exp(-(double)p[i]/3500000); //Posterior probability aproximation
}

char *recNN::strClass(int c) {
  return (char *)(key2cl[c]).c_str();
}

int recNN::keyClass(char *str) {
  if( cl2key.find(str) == cl2key.end() ) {
    fprintf(stderr, "Warning: Class '%s' doesn't appear in symbols database\n", str);
    return -1;
  }
  return cl2key[str];
}

int recNN::getNClasses() {
  return C;
}

int recNN::symType(int k) {
  return type[k];
}
