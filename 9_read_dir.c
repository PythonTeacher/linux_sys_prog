#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

char *get_type_name(int type) {
  switch(type) {
    case DT_REG:
      return "regular file";
    case DT_DIR:
      return "directory";
    case DT_LNK:
      return "symbolic link";
    default:
      return "unknown";
  }
}

int main(int argc, char **argv) {
  DIR *dp;
  struct dirent *entry;
  dp = opendir(".");
  if (!dp) {
    printf("opendir() fail\n");
    return -1;
  }
  while ((entry = readdir(dp))) {
    printf("%s: %s\n", entry->d_name, 
               get_type_name(entry->d_type));
  }
  closedir(dp);
  return 0;
}
