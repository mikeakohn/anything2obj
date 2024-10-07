/*

  text2obj - Create .o files from text files so they can be linked.

  Copyright 2024 - Michael Kohn (mike@mikekohn.net)
  https://www.mikekohn.net/

  This program falls under the MIT license.

*/

#ifndef SECTION_H
#define SECTION_H

#include <stdint.h>

struct Section
{
  Section() :
   sh_name      { 0 },
   sh_type      { 0 },
   sh_flags     { 0 },
   sh_addr      { 0 },
   sh_offset    { 0 },
   sh_size      { 0 },
   sh_link      { 0 },
   sh_info      { 0 },
   sh_addralign { 1 },
   sh_entsize   { 0 }
  {
  }

  uint32_t sh_name;
  uint32_t sh_type;
  uint64_t sh_flags;
  uint64_t sh_addr;
  uint64_t sh_offset;
  uint64_t sh_size;
  uint32_t sh_link;
  uint32_t sh_info;
  uint64_t sh_addralign;
  uint64_t sh_entsize;
};

#endif

