#include <stdio.h> 
#include <ctype.h>
#include "hex_dump.h"
#include "round.h"

/* Dumps the SIZE bytes in BUF to the console as hex bytes
   arranged 16 per line.  Numeric offsets are also included,
   starting at OFS for the first byte in BUF.  If ASCII is true
   then the corresponding ASCII characters are also rendered
   alongside. */   
void
hex_dump (uintptr_t ofs, const void *buf__, size_t size, bool ascii)
{
  const uint8_t *buf = buf__;
  const size_t per_line = 16; /* Maximum bytes per line. */

  while (size > 0)
    {
      size_t start, end, n;
      size_t i;
      
      /* Number of bytes on this line. */
      start = ofs % per_line;
      end = per_line;
      if (end - start > size)
        end = start + size;
      n = end - start;

      /* Print line. */
      printf ("%08jx  ", (uintmax_t) ROUND_DOWN (ofs, per_line));
      for (i = 0; i < start; i++)
        printf ("   ");
      for (; i < end; i++) 
        printf ("%02hhx%c",
                buf[i - start], i == per_line / 2 - 1? '-' : ' ');
      if (ascii) 
        {
          for (; i < per_line; i++)
            printf ("   ");
          printf ("|");
          for (i = 0; i < start; i++)
            printf (" ");
          for (; i < end; i++)
            printf ("%c",
                    isprint (buf[i - start]) ? buf[i - start] : '.');
          for (; i < per_line; i++)
            printf (" ");
          printf ("|");
        }
      printf ("\n");

      ofs += n;
      buf += n;
      size -= n;
    }
}
