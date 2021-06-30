#ifndef __HEX_DUMP_H
#define __HEX_DUMP_H

#include <stdbool.h>
#include <stdint.h>

void hex_dump (uintptr_t ofs, const void *buf__, size_t size, bool ascii);

#endif /* hex_dump.h */
