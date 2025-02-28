#define CBONE_IMPL
#include "cbone.h"

#define CFLAGS "-ggdb", "-Wall", "-Wextra", "-Werror", "-pedantic-errors", "-std=c99"

void build_objfile(char *tool) {
  Str_Array buildsep = make_str_array("build", tool, NULL);
  Str_Array target = make_str_array(tool, ".c", NULL);
  char *aspath = concat_str_array(path_sep, buildsep);
  char *targetdotc = concat_str_array("", target);
  Str_Array srcpath = make_str_array("src", targetdotc, NULL);
  char *srcdotc = concat_str_array(path_sep, srcpath);

  CMD(cc, "-o", aspath, srcdotc, CFLAGS);

  free(aspath);
  free(srcdotc);
  free(targetdotc);
  free(target.items);
  free(srcpath.items);
  free(buildsep.items);
}

int main(int argc, char **argv) {
  REBUILD_SELF(argc, argv);
  
  build_objfile("choi_disasm");
  build_objfile("choiasm");
  build_objfile("choivm");
  // build_tool("cinc/main");
  return 0;
}
