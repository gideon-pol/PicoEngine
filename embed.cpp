#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

FILE* open_or_exit(const char* fname, const char* mode)
{
  FILE* f = fopen(fname, mode);
  if (f == NULL) {
    perror(fname);
    exit(EXIT_FAILURE);
  }
  return f;
}

int main(int argc, char** argv)
{
  if (argc < 3) {
    fprintf(stderr, "USAGE: %s {sym} {rsrc}\n\n"
        "  Creates {sym}.c from the contents of {rsrc}\n",
        argv[0]);
    return EXIT_FAILURE;
  }

  const char* path = argv[1];
  const char* sym = argv[2];
  FILE* in = open_or_exit(argv[3], "r");

  char symfile[256];
  snprintf(symfile, sizeof(symfile), "%s/%s.c", path, sym);

  FILE* out = open_or_exit(symfile,"w");
  fprintf(out, "#include <stdlib.h>\n#include <stdint.h>\n");
  fprintf(out, "const uint8_t %s[] = {\n", sym);

  unsigned char buf[256];
  size_t nread = 0;
  size_t linecount = 0;
  do {
    nread = fread(buf, 1, sizeof(buf), in);
    size_t i;
    for (i=0; i < nread; i++) {
      fprintf(out, "0x%02x, ", buf[i]);
      if (++linecount == 10) { fprintf(out, "\n"); linecount = 0; }
    }
  } while (nread > 0);
  if (linecount > 0) fprintf(out, "\n");
  fprintf(out, "};\n");
  fprintf(out, "const size_t %s_len = sizeof(%s);\n\n",sym,sym);

  fclose(in);
  fclose(out);

  return EXIT_SUCCESS;
}