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

#ifndef _G_PARSER_
#define _G_PARSER_

class Grammar;

#include <cstdio>
#include <cstdlib>
#include "grammar.h"

class gParser{
  Grammar *g;
  char *pre;

  bool notInfoChar(char c);
  int  split(char *str,char ***res);
  bool nextLine(FILE *fd, char *lin);
  void solvePath(char *in, char *out);

public:
  gParser(Grammar *gram, FILE *fd, char *path);
  ~gParser();

  void parse(FILE *fd);
};

#endif
