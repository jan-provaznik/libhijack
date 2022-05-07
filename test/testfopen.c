/* 2018 - 2022 Jan Provaznik (jan@provaznik.pro)
 *
 * Opens a file and prints it out, byte by byte. 
 * Not overly efficient but it does its job.
 *
 * Uses fopen (3) from the standard library instead of open (2).
 */

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char ** argv) {
  FILE * stream;
  char * source;
  int letter, offset;

  for (offset = 1; offset < argc; ++offset) {
    source = *(argv + offset);
    stream = fopen(source, "r");
    while (EOF != (letter = fgetc(stream))) {
      fputc(letter, stdout);
    }
    fclose(stream);
  }

}

/* cc testfopen.c -o testfopen */

