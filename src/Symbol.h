/*

  text2obj - Create .o files from text files so they can be linked.

  Copyright 2024 - Michael Kohn (mike@mikekohn.net)
  https://www.mikekohn.net/

  This program falls under the MIT license.

*/

#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdint.h>

struct Symbol
{
  Symbol() :
   st_name  { 0 },
   st_info  { 0 },
   st_other { 0 },
   st_shndx { 0 },
   st_value { 0 },
   st_size  { 0 }
  {
  }

  uint32_t st_name;
  uint8_t  st_info;
  uint8_t  st_other;
  uint16_t st_shndx;
  uint64_t st_value;
  uint64_t st_size;
};

#endif

