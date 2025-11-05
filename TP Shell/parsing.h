#ifndef PARSING_H
#define PARSING_H

#include "defs.h"
#include "types.h"
#include "createcmd.h"
#include "utils.h"

extern int status;

struct cmd *parse_line(char *b);

#endif  // PARSING_H
