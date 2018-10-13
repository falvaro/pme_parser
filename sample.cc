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
#include <map>
#include <Magick++.h>
#include "sample.h"
#include "mfset.h"

using namespace std;
using namespace Magick;

Sample::Sample(char *str) {
  img = new Image(str);
  X=img->columns();
  Y=img->rows();
  
  data = new unsigned char*[Y];
  for(int y=0; y<Y; y++) {
    data[y] = new unsigned char[X];
    for( int x=0; x<X; x++) {
      Color pc = img->pixelColor(x,y);
      data[y][x] = 255*((float)pc.quantumRed()/QuantumRange);
    }
  }

  mfset = new MFSET(X, Y);
  int fondo=0;
  int conex[8][2] = {{-1,-1},{0,-1},{1,-1},
		     {-1, 0},	    {1, 0},
		     {-1,+1},{0,+1},{1,+1}};
  
  for(int y=0; y<Y; y++)
    for(int x=0; x<X; x++) {
      if( get(x,y)<255 ) { //If it is foreground pixel
	for(int cx=0; cx<8; cx++) {
	  int px = x+conex[cx][0];
	  int py = y+conex[cx][1];

	  if( ( px < 0 || px >= X ) 
	      || ( py < 0 || py >= Y ) )
	    continue;

	  if( get(px,py)<255 )
	    mfset->merge(x,y, px,py);
	}
      }
      else
	fondo++;
    }

  //Number of connected components found in the image
  NC = mfset->get_nSets() - fondo;
  comps = new component[NC];
  map<int,int> rComp;
  int ncomp=0;

  //Index components and compute bounding boxes
  for(int x=0; x<X; x++)
    for(int y=0; y<Y; y++)
      if( get(x,y)<255 ) {
	int rp = mfset->find(x,y);
	if( rComp.find(rp) == rComp.end() ) {
	  comps[ncomp].x = x;
	  comps[ncomp].y = y;
	  comps[ncomp].s = x;
	  comps[ncomp].t = y;
	  comps[ncomp].rp = rp;
	  rComp[rp] = ncomp++;
	}
	else {
	  int n = rComp[rp];
	  if( x < comps[n].x )
	    comps[n].x = x;
	  if( y < comps[n].y )
	    comps[n].y = y;
	  if( x > comps[n].s )
	    comps[n].s = x;
	  if( y > comps[n].t )
	    comps[n].t = y;
	}
      }

}

Sample::~Sample() {
  for(int y=0; y<Y; y++)
    delete[] data[y];
  delete[] data;
  delete mfset;
}

unsigned char Sample::get(int x, int y) {
  return data[y][x];
}

int Sample::dimX() {
  return X;
}

int Sample::dimY() {
  return Y;
}

int Sample::nComponents() {
  return NC;
}

int Sample::rp2cmp(int rp) {
  for(int i=0; i<NC; i++)
    if( comps[i].rp == rp )
      return i;

  return -1;
}

void Sample::getRegion(int *vec, int nComp, int *as, int *cn, int *ds) {
  getRegion(vec, comps[nComp].x, comps[nComp].y,
	         comps[nComp].s, comps[nComp].t, comps[nComp].rp);

 
  int n=0, cen=0;
  float asc=0, des=0;
  float wasc=0.1, wdes=1.9;
  float paso=1.8/(comps[nComp].t-comps[nComp].y);

  for(int y=comps[nComp].y; y<=comps[nComp].t; y++) {
    for(int x=comps[nComp].x; x<=comps[nComp].s; x++)
      if( get(x,y)<255 && mfset->find(x,y)==comps[nComp].rp ) {
	n++;
	asc += y*wasc;
	cen += y;
	des += y*wdes;
      }
    wasc+=paso;
    wdes-=paso;
  }

  *as = asc/n;
  *cn = cen/n;
  *ds = des/n;
}

void Sample::getRegion(int *vec, int nComp, int otroRp,
			int *as, int *cn, int *ds) {
  int otrCmp = rp2cmp(otroRp);

  int bbx, bby, bbs, bbt;
  bbx = min(comps[nComp].x,comps[otrCmp].x);
  bby = min(comps[nComp].y,comps[otrCmp].y);
  bbs = max(comps[nComp].s,comps[otrCmp].s);
  bbt = max(comps[nComp].t,comps[otrCmp].t);

  getRegion(vec, bbx, bby, bbs, bbt, comps[nComp].rp, otroRp);

  int n=0, cen=0;
  float asc=0, des=0;
  float wasc=0.1, wdes=1.9;
  float paso=1.8/(bbt-bby);

  for(int y=bby; y<=bbt; y++) {
    for(int x=bbx; x<=bbs; x++)
      if( get(x,y)<255 ) {
	int cc = mfset->find(x,y);
	if( cc==comps[nComp].rp || cc==otroRp ) {
	  n++;
	  asc += y*wasc;
	  cen += y;
	  des += y*wdes;
	}
      }
    wasc+=paso;
    wdes-=paso;
  }

  *as = asc/n;
  *cn = cen/n;
  *ds = des/n;
}

//Get the (x,y)-(s,t) region normalized to 15x15 and it is stored in 'vec'
void Sample::getRegion(int *vec, int x, int y, int s, int t, int rp1, int rp2) {
  //Create a copy of the image
  Image reg(*img);
  
  //Get the region (x,y)-(s,t)
  reg.chop( Geometry(x,y) );
  reg.crop( Geometry(s-x+1, t-y+1) );
  
  //Remove the pixels of the region that don't belong
  //to any connected component of rp1 or rp2 from the MFSET
  Color white(QuantumRange,QuantumRange,QuantumRange);
  for(int i=y; i<=t; i++)
    for(int j=x; j<=s; j++) {
      int rp = mfset->find(j,i);
      if( rp != rp1 && rp != rp2 )
	reg.pixelColor(j-x,i-y, white);
    }
  
  //Scale image to 15x15
  reg.zoom( Geometry("15x15!") );
  
  //Store result into 'vec'
  for(int i=0; i<15; i++)
    for(int j=0; j<15; j++) {
      Color pc = reg.pixelColor(j,i);
      vec[i*15+j] = 255*((float)(pc.quantumRed())/QuantumRange);
    }
  
}

void Sample::setRegion(CYKcell *c, int nComp) {
  c->x = comps[nComp].x;
  c->y = comps[nComp].y;
  c->s = comps[nComp].s;
  c->t = comps[nComp].t;
}

void Sample::setRegion(CYKcell *c, int nComp, int otroRp) {
  int otrCmp = rp2cmp( otroRp );
  c->x = min(comps[nComp].x, comps[otrCmp].x);
  c->y = min(comps[nComp].y, comps[otrCmp].y);
  c->s = max(comps[nComp].s, comps[otrCmp].s);
  c->t = max(comps[nComp].t, comps[otrCmp].t);
}

int Sample::getCandidates(int nComp, int *cand, int dx, int dy) {
  int rx = comps[nComp].x - dx;
  int ry = comps[nComp].y - dy;
  int rs = comps[nComp].s + dx;
  int rt = comps[nComp].t + dy;

  if( rx < 0 ) rx=0;
  if( ry < 0 ) ry=0;
  if( rs > X ) rs=X;
  if( rt > Y ) rt=Y;

  int cind=0;
  for(int x=rx; x<rs; x++)
    for(int y=ry; y<rt; y++)
      if( get(x,y)<255 ) { 
	int pixr = mfset->find(x,y);
	if( rp2cmp(pixr)>=0 && pixr != comps[nComp].rp ) {
	  bool already=false;
	  for(int i=0; i<cind; i++)
	    if( cand[i] == pixr ) {
	      already = true;
	      break;
	    }

	  if( !already ) {
	    cand[cind] = pixr;
	    cind++;
	  }
	}
      }

  return cind;
}

void Sample::print() {
  printf("Sample (%d x %d)\n", X, Y);
  printf("Number of components: %d\n", NC);

  for(int i=0; i<NC; i++) {
    printf("Component %d: (%d,%d)-(%d,%d)\n", i, comps[i].x, comps[i].y,
	   comps[i].s, comps[i].t);
  }
}
