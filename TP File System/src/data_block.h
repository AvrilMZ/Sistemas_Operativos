#ifndef DATA_BLOCK_H
#define DATA_BLOCK_H

#include <stdint.h>
#include "config.h"

typedef struct data_block {
	uint8_t data[BLOCK_SIZE];
} data_block_t;

#endif /* DATA_BLOCK_H */