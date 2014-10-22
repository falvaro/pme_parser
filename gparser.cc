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

#include "gparser.h"
#include <cstdlib>
#include <cstring>

#define SIZE 1024


gParser::gParser(Grammar *gram, FILE *fd, char *path) {
  g = gram;

  int n = strlen(path);

  //Save the prefix to the grammar file
  if( n > 0 ) {
    pre = new char[n+1];
    strcpy(pre, path);
  }
  else {
    pre = new char[1];
    pre[0] = 0;
  }

  parse( fd );
}

gParser::~gParser() {
  delete[] pre;
}

bool gParser::notInfoChar(char c) {
  switch(c) {
  case ' ':
  case '\t':
  case '\n':
  case '\r':
    return true;
  default: 
    return false;
  }
}

//Split the string 'str' into tokens
int gParser::split(char *str,char ***res){
  char tokensaux[2*SIZE];
  int n=0, i=0, j=0;

  while( notInfoChar(str[i]) )  i++;

  while( str[i] ) {
    if( str[i] == '\"' ) {
      i++;
      while( str[i] && str[i] != '\"' ) {
	tokensaux[j] = str[i];
	i++; j++;
      }
      i++;
    }
    else {
      while( str[i] && !notInfoChar(str[i]) ) {
	tokensaux[j] = str[i];
	i++; j++;
      }
    }
    tokensaux[j++] = 0;
    n++;
    while( str[i] && notInfoChar(str[i]) )  i++;
  }

  char **toks=new char*[n];
  for(i=0, j=0; i<n; i++) {
    int tlen = strlen(&tokensaux[j])+1;
    toks[i] = new char[tlen];
    strcpy(toks[i], &tokensaux[j]);
    j += tlen;
  }

  *res = toks;

  return n;
}


//Read next line skipping comments
bool gParser::nextLine(FILE *fd, char *lin) {
  do{
    if( fgets(lin, SIZE, fd) == NULL )
      return false;
  }while( lin[0]=='#' || strlen(lin)<=1 );

  return true;
}

//Function to solve relative paths
void gParser::solvePath(char *in, char *out) {
  strcpy(out, pre); //Copy the prefix
  strcat(out, in);  //Add the rest of the path
  out[strlen(out)-1] = 0; //Remove the final '\n'
}


void gParser::parse(FILE *fd) {
  char line[SIZE], tok1[SIZE], tok2[SIZE], aux[SIZE];

  if( !nextLine(fd, line) ) {
    fprintf(stderr, "Error: Invalid grammar\n");
    exit(-1);
  }

  //Add the prefix
  solvePath(line, tok1);
  nextLine(fd, line);
  solvePath(line, tok2);

  //Set the symbol classifier
  g->setSims(tok1, tok2);

  //Read nonterminal symbols
  while( nextLine(fd, line) && strcmp(line, "START\n") ) {
    sscanf(line, "%s", tok1);
    g->addNoTerminal(tok1);
  }

  //Read initial symbols
  while( nextLine(fd, line) && strcmp(line, "PTERM\n") ) {
    sscanf(line, "%s", tok1);
    g->addInitSym(tok1);
  }

  //Read terminal productions
  while( nextLine(fd, line) && strcmp(line, "PBIN\n") ) {
    sscanf(line, "%s %s", tok1, tok2);

    //Add the prefix
    strcpy(aux, pre);
    strcat(aux, tok2);

    //Add terminal to the grammar
    g->addTerminal(tok1, aux);
  }

  //Read binary productions
  while( nextLine(fd, line) ) {
    char **tokens;
    int ntoks = split(line, &tokens);

    if( ntoks != 6 &&
	(ntoks!=7 || (strcmp(tokens[1], "V") && strcmp(tokens[1], "Vs")) ) ) {
      fprintf(stderr, "Error: Invalid grammar (PBIN)\n");
      exit(-1);
    }

    if( !strcmp(tokens[1], "H") )
      g->addRuleH(atof(tokens[0]), tokens[2], tokens[3], tokens[4], tokens[5]);
    else if( !strcmp(tokens[1], "V") )
      g->addRuleV(atof(tokens[0]), tokens[2], tokens[3], tokens[4], tokens[5], tokens[6]);
    else if( !strcmp(tokens[1], "Vs") )
      g->addRuleVs(atof(tokens[0]), tokens[2], tokens[3], tokens[4], tokens[5], tokens[6]);
    else if( !strcmp(tokens[1], "Sup") )
      g->addRuleSup(atof(tokens[0]), tokens[2], tokens[3], tokens[4], tokens[5]);
    else if( !strcmp(tokens[1], "Sub") )
      g->addRuleSub(atof(tokens[0]), tokens[2], tokens[3], tokens[4], tokens[5]);
    else if( !strcmp(tokens[1], "SSE") )
      g->addRuleSSE(atof(tokens[0]), tokens[2], tokens[3], tokens[4], tokens[5]);
    else if( !strcmp(tokens[1], "Ins") )
      g->addRuleIns(atof(tokens[0]), tokens[2], tokens[3], tokens[4], tokens[5]);
    else {
      fprintf(stderr, "Error: Invalid rule type '%s'\n", tokens[1]);
      exit(-1);
    }
   
    //Free memory
    for(int j=0; j<ntoks; j++)
      delete[] tokens[j];
    delete[] tokens;
  }
}
