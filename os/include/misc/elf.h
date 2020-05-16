#ifndef _ELF_H
#define _ELF_H

#include <stdint.h>
#include <stddef.h>
#include <fs/vfs.h>

#define ELF_NIDENT	        16
#define EFL_FILE_LOCATION   0x30000000
# define ELFMAG0	        0x7F // e_ident[EI_MAG0]
# define ELFMAG1	        'E'  // e_ident[EI_MAG1]
# define ELFMAG2	        'L'  // e_ident[EI_MAG2]
# define ELFMAG3	        'F'  // e_ident[EI_MAG3]
 
# define ELFDATA2LSB	    (1)  // Little Endian
# define ELFCLASS32	        (1)  // 32-bit Architecture
# define EM_386		        (3)  // x86 Machine Type
# define EV_CURRENT	        (1)  // ELF Current Version
# define SHN_UNDEF	        (0x00) // Undefined/Not present


typedef uint16_t Elf32_Half; // Unsigned half int
typedef uint32_t Elf32_Off;  // Unsigned offset
typedef uint32_t Elf32_Addr; // Unsigned address
typedef uint32_t Elf32_Word; // Unsigned int
typedef int32_t Elf32_Sword; // Signed int


 
typedef struct {
	uint8_t		e_ident[ELF_NIDENT];
	Elf32_Half	e_type;
	Elf32_Half	e_machine;
	Elf32_Word	e_version;
	Elf32_Addr	e_entry;
	Elf32_Off	e_phoff;
	Elf32_Off	e_shoff;
	Elf32_Word	e_flags;
	Elf32_Half	e_ehsize;
	Elf32_Half	e_phentsize;
	Elf32_Half	e_phnum;
	Elf32_Half	e_shentsize;
	Elf32_Half	e_shnum;
	Elf32_Half	e_shstrndx;
} Elf32_Header;

enum Elf_Ident {
	EI_MAG0		    = 0, // 0x7F
	EI_MAG1		    = 1, // 'E'
	EI_MAG2		    = 2, // 'L'
	EI_MAG3		    = 3, // 'F'
	EI_CLASS	    = 4, // Architecture (32/64)
	EI_DATA		    = 5, // Byte Order
	EI_VERSION	    = 6, // ELF Version
	EI_OSABI	    = 7, // OS Specific
	EI_ABIVERSION	= 8, // OS Specific
	EI_PAD		    = 9  // Padding
};
 
enum Elf_Type {
	ET_NONE		= 0, // Unkown Type
	ET_REL		= 1, // Relocatable File
	ET_EXEC		= 2  // Executable File
};

typedef struct {
	Elf32_Word	sh_name;
	Elf32_Word	sh_type;
	Elf32_Word	sh_flags;
	Elf32_Addr	sh_addr;
	Elf32_Off	sh_offset;
	Elf32_Word	sh_size;
	Elf32_Word	sh_link;
	Elf32_Word	sh_info;
	Elf32_Word	sh_addralign;
	Elf32_Word	sh_entsize;
} Elf32_Shdr;
 
enum ShT_Types {
	SHT_NULL	= 0,   // Null section
	SHT_PROGBITS	= 1,   // Program information
	SHT_SYMTAB	= 2,   // Symbol table
	SHT_STRTAB	= 3,   // String table
	SHT_RELA	= 4,   // Relocation (w/ addend)
	SHT_NOBITS	= 8,   // Not present in file
	SHT_REL		= 9,   // Relocation (no addend)
};
 
enum ShT_Attributes {
	SHF_WRITE	= 0x01, // Writable section
	SHF_ALLOC	= 0x02  // Exists in memory
};



uint32_t exec(char *filename, uint32_t argc, char **argv);

#endif