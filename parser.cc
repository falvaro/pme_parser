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
#include "grammar.h"

using namespace std;

int main(int argc, char *argv[]) {
  if( argc != 3 ) {
    fprintf(stderr, "Usage: %s grammar file\n", argv[0]);
    return -1;
  }

  //Check file
  FILE *fpars = fopen(argv[2], "r");
  if( !fpars ) {
    fprintf(stderr, "Error loading file '%s'\n", argv[2]);
    return -1;
  }
  fclose(fpars);

  //Load grammar
  Grammar gram(argv[1]);

  //Load sample
  Sample m(argv[2]);
    
  //Print sample information
  m.print();
    
  //Parse sample
  gram.parse(&m);

  return 0;
}
