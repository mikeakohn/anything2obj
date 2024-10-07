/*

  text2obj - Create .o files from text files so they can be linked.

  Copyright 2024 - Michael Kohn (mike@mikekohn.net)
  https://www.mikekohn.net/

  This program falls under the MIT license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Elf.h"

int main(int argc, char *argv[])
{
  FILE *in;
  Elf elf;
  const char *obj_name = nullptr;
  const char *input_name = nullptr;
  const char *var_name = nullptr;

  if (argc == 1)
  {
    printf(
      "Usage: %s [ options ] <outfile.o>\n"
      "     -i <input file>\n"
      "     -v <variable name>\n",
      argv[0]);
    return 0;
  }

  for (int n = 1; n < argc; n++)
  {
    if (argv[n][0] == '-')
    {
      if (n + 1 >= argc)
      {
        printf("Error: Option requires value.\n");
        exit(1);
      }

      if (strcmp(argv[n], "-i") == 0)
      {
        input_name = argv[++n];
      }
        else
      if (strcmp(argv[n], "-v") == 0)
      {
        var_name = argv[++n];
      }
        else
      {
        printf("Error: Unknown option %s\n", argv[n]);
        exit(1);
      }
    }
      else
    {
      obj_name = argv[n];
    }
  }

  if (input_name == nullptr)
  {
    printf("Error: No input file.\n");
    exit(1);
  }

  if (var_name == nullptr)
  {
    printf("Error: No variable name.\n");
    exit(1);
  }

  if (obj_name == nullptr)
  {
    printf("Error: No output file.\n");
    exit(1);
  }

  in = fopen(input_name, "rb");

  if (in == NULL)
  {
    printf("Error: Cannot open file %s for reading.\n", input_name);
    exit(1);
  }

  if (elf.create(obj_name) != 0)
  {
    printf("Error: Cannot open file %s for writing.\n", obj_name);
    exit(1);
  }


  printf(" Input File: %s\n", input_name);
  printf("Output File: %s\n", obj_name);
  printf("Symbol Name: %s\n", var_name);

  elf.add_string(input_name);
  elf.add_string(var_name);
  elf.write_data(in);
  elf.write_sections();

  fclose(in);

  return 0;
}

