/*

  text2obj - Create .o files from text files so they can be linked.

  Copyright 2024 - Michael Kohn (mike@mikekohn.net)
  https://www.mikekohn.net/

  This program falls under the MIT license.

*/

#ifndef ELF_H
#define ELF_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "Section.h"
#include "Symbol.h"

class Elf
{
public:
  Elf();
  ~Elf();

  int create(const char *filename);
  int write_sections();
  int write_data(FILE *in);

  int add_string(const char *name);
  int add_section_name(const char *name);

private:
  int write_header();
  int write_section(Section &section);
  int write_symbol(Symbol &symbol);
  int write_symbols();

  struct TableEntry
  {
    int index;
    std::string name;
  };

  int write_string_table(std::vector<TableEntry> &table);
  int get_section_name(const char *name);
  int get_string_name(const char *name);

  virtual int write_int64(uint64_t value);
  virtual int write_int32(uint32_t value);
  virtual int write_int16(uint16_t value);
  virtual int write_int8(uint8_t value);
  virtual int write_bytes(uint8_t *data, int length);

  FILE *out;

  // ELF header.
  uint8_t e_ident[16];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;

  // ELF section.
  Section section_null;
  Section section_rodata;
  Section section_data_rel_local;
  Section section_rela_data_rel_local;
  Section section_note_gnu_stack;
  Section section_symtab; // FIXME: Add this
  Section section_strtab;
  Section section_shstrtab;

  std::vector<TableEntry> strtab;
  std::vector<TableEntry> shstrtab;

  int strtab_index;
  int shstrtab_index;

  const int rodata_ndx              = 1;
  const int data_rel_local_ndx      = 2;
  const int rela_data_rel_local_ndx = 3;
  const int note_gnu_stack_ndx      = 4;
  const int symtab_ndx              = 5;
  const int strtab_ndx              = 6;
  const int shstrtab_ndx            = 7;

  //std::string filename;
};

#endif

