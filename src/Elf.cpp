/*

  text2obj - Create .o files from text files so they can be linked.

  Copyright 2024 - Michael Kohn (mike@mikekohn.net)
  https://www.mikekohn.net/

  This program falls under the MIT license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Elf.h"
#include "Section.h"
#include "Symbol.h"

Elf::Elf() :
  out            { NULL },
  e_type         {    1 },
  e_machine      { 0x3e },
  e_version      {    1 },
  e_entry        {    0 },
  e_phoff        {    0 },
  e_shoff        {    0 },
  e_flags        {    0 },
  e_ehsize       { 0x40 },
  e_phentsize    {    0 },
  e_phnum        {    0 },
  e_shentsize    {   64 },
  e_shnum        {    0 },
  e_shstrndx     {    0 },
  strtab_index   {    1 },
  shstrtab_index {    1 }
{
  // Default: 64 bit, Little Endian, Unix Sys-V ABI.
  // e_type 1 is relocatable.
  // e_machine 0x3e is x86-64.
  const uint8_t header[] = { 0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01 };

  memset(e_ident, 0, sizeof(e_ident));
  memcpy(e_ident, header, sizeof(header));
}

Elf::~Elf()
{
  if (out != NULL) { fclose(out); }
}

int Elf::create(const char *filename)
{
  out = fopen(filename, "wb+");

  if (out == NULL) { return -1; }

  write_header();

  //add_section_name(".text");
  add_section_name(".rodata");
  add_section_name(".data.rel.local");
  add_section_name(".rela.data.rel.local");
  add_section_name(".note.GNU-stack");
  add_section_name(".symtab");
  add_section_name(".strtab");
  add_section_name(".shstrtab");

  return 0;
}

int Elf::write_data(FILE *in)
{
  uint8_t buffer[8192];

  section_rodata.sh_name   = get_section_name(".rodata");
  section_rodata.sh_offset = ftell(out);
  section_rodata.sh_type   = 1;
  section_rodata.sh_flags  = 2;

  while (true)
  {
    int n = fread(buffer, 1, sizeof(buffer), in);

    if (n == 0) { break; }

    fwrite(buffer, 1, n, out);
  }

  section_rodata.sh_size = ftell(in);

  while ((ftell(out) % 8) != 0) { putc(0, out); }

  return 0;
}

int Elf::write_sections()
{
  write_symbols();

  section_data_rel_local.sh_name      = get_section_name(".data.rel.local");
  section_data_rel_local.sh_type      = 1;
  section_data_rel_local.sh_flags     = 3;
  section_data_rel_local.sh_offset    = ftell(out);
  section_data_rel_local.sh_size      = 8;
  section_data_rel_local.sh_addralign = 8;

  write_int64(0);

  section_rela_data_rel_local.sh_name      = get_section_name(".rela.data.rel.local");
  section_rela_data_rel_local.sh_type      = 4;
  section_rela_data_rel_local.sh_flags     = 0x40;
  section_rela_data_rel_local.sh_offset    = ftell(out);
  section_rela_data_rel_local.sh_size      = 24;
  section_rela_data_rel_local.sh_link      = symtab_ndx;
  section_rela_data_rel_local.sh_info      = data_rel_local_ndx;
  section_rela_data_rel_local.sh_addralign = 8;
  section_rela_data_rel_local.sh_entsize   = 24;

  write_int64(0);

  write_int8(1);
  write_int8(0);
  write_int8(0);
  write_int8(0);

  write_int8(2);
  write_int8(0);
  write_int8(0);
  write_int8(0);

  write_int64(0);

  section_note_gnu_stack.sh_name      = get_section_name(".note.GNU-stack");
  section_note_gnu_stack.sh_type      = 1;
  section_note_gnu_stack.sh_flags     = 0x40;
  section_note_gnu_stack.sh_offset    = ftell(out);
  section_note_gnu_stack.sh_addralign = 1;

  section_strtab.sh_name     = get_section_name(".strtab");
  section_strtab.sh_type     = 3;
  section_strtab.sh_offset   = write_string_table(strtab);
  section_strtab.sh_size     = strtab_index;

  section_shstrtab.sh_name   = get_section_name(".shstrtab");
  section_shstrtab.sh_type   = 3;
  section_shstrtab.sh_offset = write_string_table(shstrtab);
  section_shstrtab.sh_size   = shstrtab_index;

  e_shstrndx = shstrtab_ndx;

  e_shoff = ftell(out);

  write_section(section_null);
  write_section(section_rodata);
  write_section(section_data_rel_local);
  write_section(section_rela_data_rel_local);
  write_section(section_note_gnu_stack);
  write_section(section_symtab);
  write_section(section_strtab);
  write_section(section_shstrtab);

  write_header();

  return 0;
}

int Elf::add_string(const char *name)
{
  TableEntry entry;

  entry.index = strtab_index;
  entry.name = name;

  strtab.push_back(entry);

  strtab_index += strlen(name) + 1;

  return 0;
}

int Elf::add_section_name(const char *name)
{
  TableEntry entry;

  entry.index = shstrtab_index;
  entry.name = name;

  shstrtab.push_back(entry);

  shstrtab_index += strlen(name) + 1;

  return 0;
}

int Elf::write_header()
{
  long marker = ftell(out);
  fseek(out, 0, SEEK_SET);

  write_bytes(e_ident, sizeof(e_ident));

  write_int16(e_type);
  write_int16(e_machine);
  write_int32(e_version);
  write_int64(e_entry);
  write_int64(e_phoff);
  write_int64(e_shoff);
  write_int32(e_flags);
  write_int16(e_ehsize);
  write_int16(e_phentsize);
  write_int16(e_phnum);
  write_int16(e_shentsize);
  write_int16(e_shnum);
  write_int16(e_shstrndx);

  if (marker != 0) { fseek(out, marker, SEEK_SET); }

  return 0;
}

int Elf::write_section(Section &section)
{
  e_shnum += 1;

  write_int32(section.sh_name);
  write_int32(section.sh_type);
  write_int64(section.sh_flags);
  write_int64(section.sh_addr);
  write_int64(section.sh_offset);
  write_int64(section.sh_size);
  write_int32(section.sh_link);
  write_int32(section.sh_info);
  write_int64(section.sh_addralign);
  write_int64(section.sh_entsize);

  return 0;
}

int Elf::write_symbol(Symbol &symbol)
{
  section_symtab.sh_size += section_symtab.sh_entsize;

  write_int32(symbol.st_name);
  write_int8(symbol.st_info);
  write_int8(symbol.st_other);
  write_int16(symbol.st_shndx);
  write_int64(symbol.st_value);
  write_int64(symbol.st_size);

  return 0;
}

int Elf::write_symbols()
{
  section_symtab.sh_name      = get_section_name(".symtab");
  section_symtab.sh_offset    = ftell(out);
  section_symtab.sh_type      = 2;
  section_symtab.sh_info      = strtab.size() + 1;
  section_symtab.sh_link      = strtab_ndx;
  section_symtab.sh_addralign = 8;
  section_symtab.sh_entsize   = 24;

  Symbol symbol_null;
  Symbol symbol_file;
  Symbol symbol_section;
  Symbol symbol_object;

  symbol_file.st_name = get_string_name(strtab[0].name.c_str());
  symbol_file.st_info = 4;
  symbol_file.st_shndx = 65521;

  symbol_section.st_info  = 3;
  symbol_section.st_shndx = rodata_ndx;

  symbol_object.st_name  = get_string_name(strtab[1].name.c_str());
  symbol_object.st_info  = 17;
  symbol_object.st_shndx = data_rel_local_ndx;
  symbol_object.st_size  = 8;

  write_symbol(symbol_null);
  write_symbol(symbol_file);
  write_symbol(symbol_section);
  write_symbol(symbol_object);

  return 0;
}

int Elf::write_string_table(std::vector<TableEntry> &table)
{
  long marker = ftell(out);

  putc(0, out);

  for (auto entry : table)
  {
    const char *name = entry.name.c_str();
    int length = strlen(name);

    fwrite(name, 1, length + 1, out);
  }

  while ((ftell(out) % 8) != 0) { putc(0, out); }

  return marker;
}

int Elf::get_section_name(const char *name)
{
  for (auto entry : shstrtab)
  {
    if (entry.name == name)
    {
      return entry.index;
    }
  }

  return 0;
}

int Elf::get_string_name(const char *name)
{
  for (auto entry : strtab)
  {
    if (entry.name == name)
    {
      return entry.index;
    }
  }

  return 0;
}

int Elf::write_int64(uint64_t value)
{
  putc(value & 0xff, out);
  putc((value >> 8) & 0xff, out);
  putc((value >> 16) & 0xff, out);
  putc((value >> 24) & 0xff, out);
  putc((value >> 32) & 0xff, out);
  putc((value >> 40) & 0xff, out);
  putc((value >> 48) & 0xff, out);
  putc((value >> 56) & 0xff, out);

  return 0;
}

int Elf::write_int32(uint32_t value)
{
  putc(value & 0xff, out);
  putc((value >> 8) & 0xff, out);
  putc((value >> 16) & 0xff, out);
  putc((value >> 24) & 0xff, out);

  return 0;
}

int Elf::write_int16(uint16_t value)
{
  putc(value & 0xff, out);
  putc((value >> 8) & 0xff, out);

  return 0;
}

int Elf::write_int8(uint8_t value)
{
  putc(value, out);

  return 0;
}

int Elf::write_bytes(uint8_t *data, int length)
{
  if (fwrite(data, 1, length, out) != (size_t)length) { return -1; }

  return 0;
}

