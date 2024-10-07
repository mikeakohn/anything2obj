#include <stdio.h>
#include <stdlib.h>

extern const char *json;

int main(int argc, char *argv[])
{
  printf("%s\n", json);

  return 0;
}

