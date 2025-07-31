#define CBONE_IMPL
#include "cbone.h"

#define CFLAGS "-ggdb", "-Wall", "-Wextra", "-Werror", "-pedantic-errors", "-std=c99", "-Wno-missing-braces"
#define FILES "parser", "string", "arena", "io", "exec", "syscall", "choi_disasm", "choiasm", "choivm"
#define BUILD_DIR "build"
#define OBJFILES \
  BUILD_DIR path_sep "parser.o", \
  BUILD_DIR path_sep "string.o", \
  BUILD_DIR path_sep "arena.o", \
  BUILD_DIR path_sep "io.o", \
  BUILD_DIR path_sep "exec.o", \
  BUILD_DIR path_sep "syscall.o"

static char *files[] = {
  FILES
};

#define nfiles 9

void build_objfile(char *tool) {
  cbone_str_array buildsep = cbone_make_str_array("build", CONCAT(tool, ".o"), NULL);
  cbone_str_array target = cbone_make_str_array(tool, ".c", NULL);
  char *aspath = cbone_concat_str_array(path_sep, buildsep);
  char *targetdotc = cbone_concat_str_array("", target);
  cbone_str_array srcpath = cbone_make_str_array("src", targetdotc, NULL);
  char *srcdotc = cbone_concat_str_array(path_sep, srcpath);

  CMD(cc, "-c", "-o", aspath, srcdotc, CFLAGS);

  free(aspath);
  free(srcdotc);
  free(targetdotc);
  free(target.items);
  free(srcpath.items);
  free(buildsep.items);
}

void build_executable(char *executable) {
  char *objexe = CONCAT("build" path_sep, executable, ".o");
  CMD(cc, "-o", executable, objexe, OBJFILES, CFLAGS);
  free(objexe);
}

int main(int argc, char **argv) {
  REBUILD_SELF(argc, argv);
  
  for (int i = 0; i < nfiles; i++) {
    build_objfile(files[i]);
  }
  build_executable("choiasm");
  build_executable("choi_disasm");
  build_executable("choivm");
  return 0;
}
