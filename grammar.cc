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
#include <cfloat>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <algorithm>
#include "grammar.h"
#include "cyktable.h"
#include "logspace.h"

using namespace std;

//Auxiliary functions

void error(const char *msg) {
  fprintf(stderr, "Grammar err[%s]\n", msg);
  exit(-1);
}

void error(const char *msg, char *str) {
  char linea[1024];
  sprintf(linea, "Grammar err[%s]\n", msg);
  fprintf(stderr, linea, str);
  exit(-1);
}

//Symbols frequently split into several connected components
bool esFreqSym(char *str) {
  if( !strcmp(str, "equal") )     return true;
  if( !strcmp(str, "i") )         return true;
  if( !strcmp(str, "j") )         return true;
  if( !strcmp(str, "colon") )     return true;
  if( !strcmp(str, "semicolon") ) return true;
  if( !strcmp(str, "leq") )       return true;
  if( !strcmp(str, "exclamation"))return true;

  return false;
}

//
//Grammar class methods
//

Grammar::Grammar(char *path) {
  FILE *fd = fopen(path, "r");
  if( !fd ) {
    fprintf(stderr, "Error loading grammar '%s'\n", path);
    exit(-1);
  }

  //Obtain the prefix to solve relative paths
  int i = strlen(path)-1;
  while( i>=0 && path[i] != '/' )
    i--;

  path[i+1] = 0;

  gParser GP(this, fd, path);
}

void Grammar::setSims(char *sims, char *info) {
  FILE *fsims=fopen(sims, "r");
  if( !fsims ) {
    fprintf(stderr, "Error loading symbols file\n");
    exit(1);
  }
  FILE *ftype=fopen(info, "r");
  if( !ftype ) {
    fprintf(stderr, "Error loading symbols information file\n");
    exit(1);
  }

  //Initialize symbol classifier
  RecSims = new recNN(fsims, ftype);
}

void Grammar::addInitSym(char *str) {
  if( nonTerminals.find(str) == nonTerminals.end() )
    error("addInitSym: Nonterminal '%s' not defined.", str);

  initsyms.push_back( nonTerminals[str] );
}

void Grammar::addNoTerminal(char *str) {
  int key = nonTerminals.size();
  nonTerminals[str] = key;
}

void Grammar::addTerminal(char *str, char *path) {
  if( nonTerminals.find(str) == nonTerminals.end() )
    error("addTerminal: Nonterminal '%s' not defined.", str);

  FILE *fd = fopen(path, "r");
  if( !fd ) {
    fprintf(stderr, "Error loading terminals file '%s'\n", path);
    exit(1);
  }

  int nclases;
  char line[512], tok[512], tex[512];
  float pr;

  fgets(line, 512, fd);
  sscanf(line, "%d", &nclases);

  ProductionT *pt = new ProductionT(nonTerminals[str], RecSims->getNClasses());
  for(int i=0; i<nclases; i++) {
    fgets(line, 512, fd);
    sscanf(line, "%f %s %s", &pr, tok, tex);

    int id=RecSims->keyClass(tok);
    if( id >= 0 )
      pt->setClass(id , pr, tex );
  }

  prodTerms.push_back( pt );
}

void Grammar::addRuleH(float pr, char *S, char *A, char *B, char *out) {
  if( nonTerminals.find(S) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", S);
  if( nonTerminals.find(A) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", A);
  if( nonTerminals.find(B) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", B);
  
  prodsH.push_back( new ProductionH(nonTerminals[S],
				    nonTerminals[A], nonTerminals[B], pr, out) );
}

void Grammar::addRuleV(float pr, char *S, char *A, char *B, char *out, char *merge) {
  if( nonTerminals.find(S) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", S);
  if( nonTerminals.find(A) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", A);
  if( nonTerminals.find(B) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", B);

  ProductionB *pd = new ProductionV(nonTerminals[S],
				    nonTerminals[A], nonTerminals[B], pr, out);

  pd->setMerges(merge[0]=='A',merge[1]=='A',merge[2]=='A');

  prodsV.push_back( pd );
}

void Grammar::addRuleVs(float pr, char *S, char *A, char *B, char *out, char *merge) {
  if( nonTerminals.find(S) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", S);
  if( nonTerminals.find(A) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", A);
  if( nonTerminals.find(B) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", B);
  
  ProductionB *pd = new ProductionVs(nonTerminals[S],
				     nonTerminals[A], nonTerminals[B], pr, out);
  
  pd->setMerges(merge[0]=='A',merge[1]=='A',merge[2]=='A');

  prodsVs.push_back( pd );
}

void Grammar::addRuleSSE(float pr, char *S, char *A, char *B, char *out) {
  if( nonTerminals.find(S) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", S);
  if( nonTerminals.find(A) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", A);
  if( nonTerminals.find(B) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", B);
  
  prodsSSE.push_back( new ProductionSSE(nonTerminals[S],
					nonTerminals[A], nonTerminals[B], pr, out) );
}

void Grammar::addRuleSup(float pr, char *S, char *A, char *B, char *out) {
  if( nonTerminals.find(S) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", S);
  if( nonTerminals.find(A) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", A);
  if( nonTerminals.find(B) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", B);
  
  prodsH.push_back( new ProductionSup(nonTerminals[S],
				      nonTerminals[A], nonTerminals[B], pr, out) );
}

void Grammar::addRuleSub(float pr, char *S, char *A, char *B, char *out) {
  if( nonTerminals.find(S) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", S);
  if( nonTerminals.find(A) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", A);
  if( nonTerminals.find(B) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", B);
  
  prodsH.push_back( new ProductionSub(nonTerminals[S],
				      nonTerminals[A], nonTerminals[B], pr, out) );
}

void Grammar::addRuleIns(float pr, char *S, char *A, char *B, char *out) {
  if( nonTerminals.find(S) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", S);
  if( nonTerminals.find(A) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", A);
  if( nonTerminals.find(B) == nonTerminals.end() )
    error("Rule: Nonterminal '%s' not defined.", B);
  
  prodsIns.push_back( new ProductionIns(nonTerminals[S],
					nonTerminals[A], nonTerminals[B], pr, out) );
}

Grammar::~Grammar() {
  for(list<ProductionB *>::iterator it=prodsH.begin(); it!=prodsH.end(); it++)
    delete *it;

  for(list<ProductionB *>::iterator it=prodsV.begin(); it!=prodsV.end(); it++)
    delete *it;

  for(list<ProductionB *>::iterator it=prodsVs.begin(); it!=prodsVs.end(); it++)
    delete *it;

  for(list<ProductionB *>::iterator it=prodsSSE.begin(); it!=prodsSSE.end(); it++)
    delete *it;

  for(list<ProductionB *>::iterator it=prodsIns.begin(); it!=prodsIns.end(); it++)
    delete *it;

  for(list<ProductionT *>::iterator it=prodTerms.begin(); it!=prodTerms.end(); it++)
    delete *it;

  delete RecSims;
}

// void Grammar::print() {
//   printf("Nonterminals %d:\n", (int)nonTerminals.size());
//   for(map<string,int>::iterator it=nonTerminals.begin();
//       it!=nonTerminals.end(); it++)
//     printf("(%s,%d)\n", it->first.c_str(), it->second);

//   int nr=0;
//   printf("\nBinary Rules %d:\n", (int)prodsH.size() + (int)prodsV.size() + (int)prodsVs.size() + (int)prodsIns.size() + (int)prodsSSE.size());
//   for(list<ProductionB *>::iterator it=prodsH.begin(); it!=prodsH.end(); it++) {
//     printf("R%dh ", nr++);
//     (*it)->print();
//   }
//   for(list<ProductionB *>::iterator it=prodsV.begin(); it!=prodsV.end(); it++) {
//     printf("R%dv ", nr++);
//     (*it)->print();
//   }

//   for(list<ProductionB *>::iterator it=prodsVs.begin(); it!=prodsVs.end(); it++) {
//     printf("R%dve ", nr++);
//     (*it)->print();
//   }
//   for(list<ProductionB *>::iterator it=prodsSSE.begin(); it!=prodsSSE.end(); it++) {
//     printf("R%dsse ", nr++);
//     (*it)->print();
//   }
//   for(list<ProductionB *>::iterator it=prodsIns.begin(); it!=prodsIns.end(); it++) {
//     printf("R%dins ", nr++);
//     (*it)->print();
//   }

//   nr=0;
//   printf("\nTerminal Rules %d:\n", (int)prodTerms.size());
//   for(list<ProductionT *>::iterator it=prodTerms.begin(); it!=prodTerms.end(); it++) {
//     printf("T%d ", nr++);
//     (*it)->print();
//   }
// }

//Combine A and B elements to generate element S given production pd (S -> A B)
CYKcell *Grammar::fusion(ProductionB *pd, CYKcell *A, CYKcell *B, int N) {
  CYKcell *S=NULL;

  //Get the combination probability according to production
  double prob = pd->prob( A, B, RX, RY );

  if( prob > 0.45 ) {
    //Get the nonterminals of the production
    int ps, pa, pb;
    pd->getData( &ps, &pa, &pb );

    if( A->compatible(pa, B, pb) && pd->getPrior() > -FLT_MAX ) {

      S = new CYKcell(nonTerminals.size(), N);

      //Compute the final log-probability
      prob = pd->getPrior() + log(prob) + A->ntsims[pa]->pr + B->ntsims[pb]->pr;

      //Compute resulting region
      S->x = min(A->x, B->x);
      S->y = min(A->y, B->y);
      S->s = max(A->s, B->s);
      S->t = max(A->t, B->t);
      
      //Create new nonterminal
      S->ntsims[ps] = new Symbol(-1, prob, N);
      pd->mergeRegions(A, B, S);

      //Set the represented components
      S->ccUnion(ps,A,pa,B,pb);

      //Save the path
      S->ntsims[ps]->hi = A;
      S->ntsims[ps]->hd = B;
      S->ntsims[ps]->prod = pd;
    }
  }

  return S;
}

//CYK table initialization by terminal mathematical symbols
void Grammar::initCYKterms(Sample *m, CYKtable *tcyk, int N, int K) {
  int vec[15*15];

  printf("\n1CC Symbols:\n");

  for(int i=0; i<N; i++) {
    int cmy, asc, des;
    m->getRegion(vec, i, &asc, &cmy, &des);
    

#ifdef VERBOSE
    printf("Component %d:\n", i);
    for(int a=0; a<15; a++) {
      for(int b=0; b<15; b++) {
	char vaux='#';
	if( vec[a*15+b] >= 250 ) vaux = ' ';
	else if( vec[a*15+b] >= 175 ) vaux = ':';
	else if( vec[a*15+b] >= 100 ) vaux = '+';
	else if( vec[a*15+b] >= 50 ) vaux = '0';

	printf("%c", vaux);
      }
      printf("\n");
    }
#endif

    CYKcell *cd = new CYKcell(nonTerminals.size(), N);
    m->setRegion(cd, i);
    
    //N-Best classification
    const int NB=10;
    int clase[NB];
    float pr[NB];

    RecSims->classify( vec, NB, clase, pr );

    float pmax=0.5;
    for(list<ProductionT *>::iterator it=prodTerms.begin(); it!=prodTerms.end(); it++) {
      ProductionT *prod = *it;

      for(int k=0; k<NB; k++)
	if( prod->getClass( clase[k] ) && pr[k] > pmax && prod->getPrior(clase[k]) > -FLT_MAX ) {
	  //Create new symbol
	  cd->ntsims[prod->getNoTerm()] = new Symbol(clase[k],
						      prod->getPrior(clase[k])+log(pr[k]), N);
	  cd->ntsims[prod->getNoTerm()]->pt = prod;
	  cd->ntsims[prod->getNoTerm()]->ccc[i] = true;

	  //Select the vertical centroid according to symbol type
	  int cen, type = RecSims->symType(clase[k]);
	  if( type==0 )       cen = cmy; //Normal
	  else if ( type==1 ) cen = asc; //Ascendant
	  else                cen = des; //Descending
	
	  //Set the corresponding baselines
	  
	  //Central baseline
	  cd->ntsims[prod->getNoTerm()]->lbhor = cen;
	  cd->ntsims[prod->getNoTerm()]->rbhor = cen;

	  //Upper Baseline
	  if( type!=1 ) {
	    cd->ntsims[prod->getNoTerm()]->lbsup = cd->y + 0.1*(cen-cd->y);
	    cd->ntsims[prod->getNoTerm()]->rbsup = cd->ntsims[prod->getNoTerm()]->lbsup;
	  }
	  else {
	    cd->ntsims[prod->getNoTerm()]->lbsup = (cd->y + cen)/2;
	    cd->ntsims[prod->getNoTerm()]->rbsup = cd->ntsims[prod->getNoTerm()]->lbsup;
	  }
	  
	  //Lower Baseline
	  if( type!=2 ) {
	    cd->ntsims[prod->getNoTerm()]->lbsub = cen + 0.9*(cd->t-cen);
	    cd->ntsims[prod->getNoTerm()]->rbsub = cd->ntsims[prod->getNoTerm()]->lbsub;
	  }
	  else {
	    cd->ntsims[prod->getNoTerm()]->lbsub = (cen + cd->t)/2;
	    cd->ntsims[prod->getNoTerm()]->rbsub = cd->ntsims[prod->getNoTerm()]->lbsub;
	  }
	}
    }

    //Add to table (size=1)
    tcyk->add(1, cd);
    
    //Print components information
    for(int j=0; j<K; j++) {
      if( cd->ntsims[j] ) {
        printf("%d_%d_%d_%d %.8f [%d] %s\n", cd->x, cd->y, cd->s, cd->t,
	       exp(cd->ntsims[j]->pr), j, RecSims->strClass(cd->ntsims[j]->clase));
      }
    }
  }

}

//Compute the dimensions of the reference symbol. It makes the parser
//independent from image resolution
void Grammar::detRefSymbol(CYKtable *tcyk) {
  vector<int> vmedx, vmedy;
  int nregs=0, lAr;
  float mAr=0;
  RX=0, RY=0;

  for(CYKcell *r=tcyk->get(1); r; r=r->next) {
    int width = r->s - r->x;
    int height = r->t - r->y;
    float ratio = (float)width/height;
    int area = width*height;

    vmedx.push_back(width);
    vmedy.push_back(height);

    mAr += area;
    if( ratio >= 0.5 && ratio <= 1.5 ) {
      RX += width;
      RY += height;
      nregs++;
    }
  }

  //mean(area)
  mAr /= vmedx.size();
  lAr = (int)(sqrt(mAr)+0.5);
  lAr*=0.9;

  //mean(RX,RY)
  if( nregs > 0 ) {
    RX /= nregs;
    RY /= nregs;
  }
  else {
    for(CYKcell *r=tcyk->get(1); r; r=r->next) {
      int width = r->s - r->x;
      int height = r->t - r->y;

      RX += width;
      RY += height;
      nregs++;
    }
    RX /= nregs;
    RY /= nregs;
  }

  //median(RX,RY)
  sort(vmedx.begin(),vmedx.end());
  sort(vmedy.begin(),vmedy.end());

  //printf("\nmean(X,Y) = (%d,%d)\n", RX, RY);
  //printf("median(X,Y) = (%d,%d)\n", vmedx[vmedx.size()/2], vmedy[vmedy.size()/2]);
  //printf("mean(area) = %.2f (%d x %d)\n", mAr, lAr, lAr);

  //The reference is computed as: max(mean(RX,RY),median(RX,RY),mean(area))
  RX = max(max(RX,vmedx[vmedx.size()/2]), lAr);
  RY = max(max(RY,vmedy[vmedy.size()/2]), lAr);

  printf("\nReference symbol:\n");
  printf("(RX,RY) = (%d,%d)\n", RX, RY);
}


//Compose symbols combining nearby connected components
void Grammar::mergeCC(Sample *m, CYKtable *tcyk, int N) {
  int vec[255];
  int *cand = new int[N];

  printf("\n2CC Symbols:\n");
  for(int i=0; i<N; i++) {
    //Get the list of components candidate to combine with  component 'i'
    int nc = m->getCandidates(i, cand, RX/2, RY);

    for(int j=0; j<nc; j++) {
      if( i > m->rp2cmp(cand[j]) )
	continue; //Avoid processing twice each i-j combination
 
      //Get the combined and scaled component in 'vec'
      int asc, cmy, des;
      m->getRegion(vec, i, cand[j], &asc, &cmy, &des);

      CYKcell *cd = new CYKcell(nonTerminals.size(), N);
      m->setRegion(cd, i, cand[j]);

      //N-Best classification
      const int NB=5;
      int clase[NB];
      float pr[NB];

      RecSims->classify( vec, NB, clase, pr );

      float pmax= (pr[NB-1] > 0.6) ? pr[NB-1]-0.1 : 0.5;
      bool combined=false;
      for(list<ProductionT *>::iterator it=prodTerms.begin(); it!=prodTerms.end(); it++) {
	ProductionT *prod = *it;

	for(int k=0; k<NB; k++)
	  if( prod->getClass( clase[k] ) && pr[k] > pmax && prod->getPrior(clase[k]) > -FLT_MAX ) {
	    //Increase probability of frequent combinations
	    if( pr[k] > 0.7 && esFreqSym(RecSims->strClass(clase[k])) ) {
	      pr[k] *= 1.1;
	      if( pr[k] > 1.0 )
		pr[k] = 1.0;
	    }

	    if( pr[k] >= 0.65  && prod->getPrior(clase[k]) > -FLT_MAX ) {
	      //Naive probability scaling
	      pr[k] = pr[k]*pr[k]*pr[k];
	      
	      //Select the vertical centroid according to symbol type
	      int cen, type = RecSims->symType(clase[k]);
	      if( type==0 )       cen = cmy; //Normal
	      else if ( type==1 ) cen = asc; //Ascendant
	      else                cen = des; //Descending

	      cd->ntsims[prod->getNoTerm()] = new Symbol(clase[k], 
							  prod->getPrior(clase[k])+log(pr[k]), N);
	      cd->ntsims[prod->getNoTerm()]->pt = prod;
	      cd->ntsims[prod->getNoTerm()]->ccc[i] = true;
	      cd->ntsims[prod->getNoTerm()]->ccc[m->rp2cmp(cand[j])] = true;
	      //Central baseline
	      cd->ntsims[prod->getNoTerm()]->lbhor = cen;
	      cd->ntsims[prod->getNoTerm()]->rbhor = cen;
	      //Upper baseline
	      if( type!=1 ) {
		cd->ntsims[prod->getNoTerm()]->lbsup = cd->y + 0.1*(cen-cd->y);
		cd->ntsims[prod->getNoTerm()]->rbsup = cd->ntsims[prod->getNoTerm()]->lbsup;
	      }
	      else {
		cd->ntsims[prod->getNoTerm()]->lbsup = (cd->y + cen)/2;
		cd->ntsims[prod->getNoTerm()]->rbsup = cd->ntsims[prod->getNoTerm()]->lbsup;
	      }
	      //Lower baseline
	      if( type!=2 ) {
		cd->ntsims[prod->getNoTerm()]->lbsub = cen + 0.9*(cd->t-cen);
		cd->ntsims[prod->getNoTerm()]->rbsub = cd->ntsims[prod->getNoTerm()]->lbsub;
	      }
	      else {
		cd->ntsims[prod->getNoTerm()]->lbsub = (cen + cd->t)/2;
		cd->ntsims[prod->getNoTerm()]->rbsub = cd->ntsims[prod->getNoTerm()]->lbsub;
	      }
	      
	      combined=true;
	      
	      printf("%d_%d_%d_%d %.8f [%d] %s\n", cd->x, cd->y, cd->s, cd->t,
		     exp(cd->ntsims[prod->getNoTerm()]->pr), prod->getNoTerm(),
		     RecSims->strClass(cd->ntsims[prod->getNoTerm()]->clase));
	    }
	  }
      }
      
      if( combined ) //Add to parsing table (size=2)
	tcyk->add(2, cd);
      else
	delete cd;
    }
  }

  delete[] cand;
}

void Grammar::parse(Sample *m) {
  int N = m->nComponents();
  int K = nonTerminals.size();

  //Cocke-Younger-Kasami (CYK) algorithm for 2D SCFG

  CYKtable tcyk( N, K );

  //CYK table initialization
  initCYKterms(m, &tcyk, N, K);

  //Compute reference symbol
  detRefSymbol( &tcyk );

  //Compose symbols combining nearby connected components
  mergeCC(m, &tcyk, N);

  LogSpace **logspace = new LogSpace*[N];
  list<CYKcell*> c1setH, c1setV, c1setU, c1setI; 

  //Initialization of spatial data structure for size=1
  logspace[1] = new LogSpace(tcyk.get(1), N, RX, RY);

  printf("\nCYK parsing:\n");

  for(int tsize=2; tsize<=N; tsize++) {

    for(int a=1; a<tsize; a++) {
      int b = tsize-a;

      for(CYKcell *c1=tcyk.get(a); c1; c1=c1->next) {

	c1setH.clear();
	c1setV.clear();
	c1setU.clear();
	c1setI.clear();

	//Get the subset of regions related to region 'c1'
	logspace[b]->getH(c1, &c1setH); //Horizontal direction
	logspace[b]->getV(c1, &c1setV); //Vertical direction (down)
	logspace[b]->getU(c1, &c1setU); //Vertical direction (up)
	logspace[b]->getI(c1, &c1setI); //Inside

	for(list<CYKcell*>::iterator c2=c1setH.begin(); c2!=c1setH.end(); c2++) {
	  for(list<ProductionB*>::iterator it=prodsH.begin(); it!=prodsH.end(); it++) {
	    //Production S -> A B
	    int pa, pb;
	    ((ProductionB*)*it)->getData( NULL, &pa, &pb );

	    if( c1->ntsims[ pa ] && (*c2)->ntsims[ pb ] ) {
	      CYKcell *cd = fusion(*it, c1, *c2, N);
	      
	      if( cd )
		tcyk.add(tsize, cd); //Add new hypothesis to the table
	    }
	  }
	}

	for(list<CYKcell*>::iterator c2=c1setV.begin(); c2!=c1setV.end(); c2++) {
	  for(list<ProductionB*>::iterator it=prodsV.begin(); it!=prodsV.end(); it++) {
	    //Production S -> A B
	    int pa, pb;
	    ((ProductionB*)*it)->getData( NULL, &pa, &pb );

	    if( c1->ntsims[ pa ] && (*c2)->ntsims[ pb ] ) {
	      CYKcell *cd = fusion(*it, c1, *c2, N);
		
	      if( cd )
		tcyk.add(tsize, cd); //Add new hypothesis to the table
	    }
	  }

	  for(list<ProductionB*>::iterator it=prodsVs.begin();
	      it!=prodsVs.end(); it++) {
	    //Production S -> A B
	    int pa, pb;
	    ((ProductionB*)*it)->getData( NULL, &pa, &pb );
	    
	    if( c1->ntsims[ pa ] && (*c2)->ntsims[ pb ] ) {
	      CYKcell *cd = fusion(*it, c1, *c2, N);
	      
	      if( cd )
		tcyk.add(tsize, cd); //Add new nypothesis to the table
	    }
	  }

	  for(list<ProductionB*>::iterator it=prodsSSE.begin();
	      it!=prodsSSE.end(); it++) {
	    //Production S -> A B
	    int pa, pb;
	    ((ProductionB*)*it)->getData( NULL, &pa, &pb );
	    
	    if( c1->ntsims[ pa ] && (*c2)->ntsims[ pb ] ) {
	      CYKcell *cd = fusion(*it, c1, *c2, N);
	      
	      if( cd )
		tcyk.add(tsize, cd); //Add new hypothesis to the table
	    }
	  }
	}

	for(list<CYKcell*>::iterator c2=c1setU.begin(); c2!=c1setU.end(); c2++) {
	  for(list<ProductionB*>::iterator it=prodsV.begin(); it!=prodsV.end(); it++) {
	    //Production S -> A B
	    int pa, pb;
	    ((ProductionB*)*it)->getData( NULL, &pa, &pb );
	    
	    if( c1->ntsims[ pb ] && (*c2)->ntsims[ pa ] ) {
	      CYKcell *cd = fusion(*it, *c2, c1, N);
	      
	      if( cd )
		tcyk.add(tsize, cd); //Add new hypothesis to the table
	    }
	  }

	  for(list<ProductionB*>::iterator it=prodsSSE.begin();
	      it!=prodsSSE.end(); it++) {
	    //Production S -> A B
	    int pa, pb;
	    ((ProductionB*)*it)->getData( NULL, &pa, &pb );
	    
	    if( c1->ntsims[ pb ] && (*c2)->ntsims[ pa ] ) {
	      CYKcell *cd = fusion(*it, *c2, c1, N);
	      
	      if( cd )
		tcyk.add(tsize, cd); //Add new hypothesis to the table
	    }
	  }
	}

	for(list<CYKcell*>::iterator c2=c1setI.begin(); c2!=c1setI.end(); c2++) {
	  for(list<ProductionB*>::iterator it=prodsIns.begin(); it!=prodsIns.end(); it++) {
	    //Production S -> A B
	    int pa, pb;
	    ((ProductionB*)*it)->getData( NULL, &pa, &pb );

	    if( c1->ntsims[ pa ] && (*c2)->ntsims[ pb ] ) {
	      CYKcell *cd = fusion(*it, c1, *c2, N);
		
	      if( cd )
		tcyk.add(tsize, cd); //Add new hypothesis to the table
	    }
	  }
	}

      }


    } //for 1 <= a < tsize

#ifdef VERBOSE
    printf("Size %d:\n", tsize);
    for(CYKcell *cp=tcyk.get(tsize); cp; cp=cp->next) {
      printf("  (%3d,%3d)-(%3d,%3d) { ", cp->x, cp->y, cp->s, cp->t);
      for(int i=0; i<cp->nnt; i++)
	if( cp->ntsims[i] ) printf("%g[%d] ", cp->ntsims[i]->pr, i);
      printf("}\n");
    }
    printf("\n");
#endif

    if( tsize < N )  //Create spatial structure for the new size
      logspace[tsize] = new LogSpace(tcyk.get(tsize), tcyk.size(tsize), RX, RY);

  } //for 2 <= tsize <= N


  //Free memory
  for(int i=1; i<N; i++)
    delete logspace[i];
  delete[] logspace;

  int total=0;
  for(int i=1; i<=N; i++) {
    printf("Size %d: Nodes generated %d\n", i, tcyk.size(i));
    total += tcyk.size(i);
  }
  printf("\nTotal generated = %d\n\n", total);

  //Print LaTeX output of most probable hypothesis
  print_latex(&tcyk, N);
}


void Grammar::print_latex(CYKtable *T, int N) {
  CYKcell *cparse=T->get(N);
  int ntini=0;

  if( cparse ) {
    cparse = NULL;
    float best = -FLT_MAX;
    for(CYKcell *c1=T->get(N); c1; c1=c1->next) {
      for(list<int>::iterator it=initsyms.begin(); it!=initsyms.end(); it++) {
	if( c1->ntsims[*it] && c1->ntsims[*it]->pr > best ) {
	  cparse = c1;
	  ntini = *it;
	  best = c1->ntsims[ntini]->pr;
	}
      }
    }
    if( cparse ) {
      
      printf("Used rules:\n");
      viterbi(cparse, ntini);
      printf("\n");

      if( cparse->ntsims[ntini]->prod ) {
	printf("Symbols:\n");
	cparse->ntsims[ntini]->prod->printComps(cparse,ntini);
	printf("\nLaTeX: ");
	cparse->ntsims[ntini]->prod->printOut(cparse);
      }
      else {
	printf("LaTeX: ");
	printf("%s", cparse->ntsims[ntini]->pt->getTeX(cparse->ntsims[ntini]->clase));
      }
      printf("\n");
    }
  }

  //If no hypothesis of size=N is available, then look for hypotheses of minor size
  if( !cparse ) {
    int nsy = N-1;
    cparse=NULL;
    while( !cparse && nsy > 0 ) {
      cparse = T->get(nsy);

      if( cparse ) {
	float best=-FLT_MAX;
	CYKcell *cbest=NULL;
	for(CYKcell *c1=cparse; c1; c1=c1->next)
	  for(list<int>::iterator it=initsyms.begin(); it!=initsyms.end(); it++) {
	    if( c1->ntsims[*it] && c1->ntsims[*it]->pr > best ) {
	      cbest = c1;
	      ntini = *it;
	      best = c1->ntsims[ntini]->pr;
	    }
	  }

	if( cbest ) {
	  cparse = cbest;
	  break;
	}
	else
	  cparse = NULL;
      }
      nsy--;
    }

    if( cparse ) {
      printf("Used rules:\n");
      viterbi(cparse, ntini);
      printf("\n");

      if( cparse->ntsims[ntini]->prod ) {
	printf("Symbols:\n");
	cparse->ntsims[ntini]->prod->printComps(cparse,ntini);
	printf("Partial Recognition (%d symbols)\n", nsy);
	printf("LaTeX: ");
	cparse->ntsims[ntini]->prod->printOut(cparse);
	printf("\n");
      }
      else {
	printf("Partial Recognition (%d symbols)\n", nsy);
	printf("LaTeX: %s\n",
	       cparse->ntsims[ntini]->pt->getTeX(cparse->ntsims[ntini]->clase));
      }
    }
    else //If any expression can be parsed, print $\emptyset$
      printf("Partial Recognition (0 symbols)\nLaTeX: \\emptyset\n");
  }
}


const char *Grammar::key2str(int k) {
  for(map<string,int>::iterator it=nonTerminals.begin(); it!=nonTerminals.end(); it++) {
    if( it->second == k )
      return it->first.c_str();
  }
  return "NULL";
}

void Grammar::viterbi(CYKcell *cell, int n) {
  if( cell->ntsims[n]->prod ) {
    //Binary Production
    int a, b;
    cell->ntsims[n]->prod->getData(NULL, &a, &b);

    printf("%%VT%% %c %s -> %s %s\n", cell->ntsims[n]->prod->type(), 
	   key2str(n), key2str(a), key2str(b));
 
    viterbi(cell->ntsims[n]->hi, a);
    viterbi(cell->ntsims[n]->hd, b);
  }
  else {
    //Terminal Production
    printf("%%VT%% T %s -> %s\n", key2str(cell->ntsims[n]->pt->getNoTerm()), 
	   cell->ntsims[n]->pt->getTeX(cell->ntsims[n]->clase));
  }
}
