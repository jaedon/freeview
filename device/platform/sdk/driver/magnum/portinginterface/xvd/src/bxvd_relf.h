/***************************************************************************
 * bxvd_elf.h
 *
 * Standard (common) definitions for ELF structures/fields.
 *
 * All of this material comes out of the ELF 1.1 Specification.
 * Metaware ARC-specific definitions are below.
 *
 * $brcm_Workfile: bxvd_relf.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 2/21/07 1:10p $
 *
 * Module Description:
 *   See module
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_relf.h $
 * 
 * Hydra_Software_Devel/7   2/21/07 1:10p pblanco
 * PR26433: Set formatting to standard agreed upon within the XVD group on
 * 2/20/07.
 * 
 * Hydra_Software_Devel/6   12/11/06 3:15p pblanco
 * PR26433: Cleaned up definitions.
 * 
 * Hydra_Software_Devel/5   8/11/06 1:53p davidp
 * PR23207: Update ARC copyright statement per Andy Jaros, VP of Sales at
 * ARC.
 * 
 * Hydra_Software_Devel/4   6/26/06 9:41a pblanco
 * PR22302: Checked in Marcin's changes to make all definitions in this
 * header XVD private.
 * 
 * Hydra_Software_Devel/3   6/22/06 2:13p pblanco
 * PR22302: Changed some typedefs to XVD local type names.
 * 
 * Hydra_Software_Devel/2   6/22/06 1:17p pblanco
 * PR20017: Incorporate Roy Lewis' changes for BE systems. Verified on LE
 * Linux and BE VxWorks.
 * 
 * Hydra_Software_Devel/1   4/4/06 2:03p pblanco
 * PR20017: Initial check in.
 *
 ***************************************************************************/
#ifndef __ELF_H__
#define __ELF_H__

typedef unsigned long   Bxvd_Elf32_Addr;
typedef unsigned short  Bxvd_Elf32_Half;
typedef unsigned long   Bxvd_Elf32_Off;
typedef signed   long   Bxvd_Elf32_Sword;
typedef unsigned long   Bxvd_Elf32_Word;

#define ET_NIDENT       16

typedef struct
{
      unsigned char   e_ident[ET_NIDENT];
      Bxvd_Elf32_Half e_type;
      Bxvd_Elf32_Half e_machine;
      Bxvd_Elf32_Word e_version;
      Bxvd_Elf32_Addr e_entry;
      Bxvd_Elf32_Off  e_phoff;
      Bxvd_Elf32_Off  e_shoff;
      Bxvd_Elf32_Word e_flags;
      Bxvd_Elf32_Half e_ehsize;
      Bxvd_Elf32_Half e_phentsize;
      Bxvd_Elf32_Half e_phnum;
      Bxvd_Elf32_Half e_shentsize;
      Bxvd_Elf32_Half e_shnum;
      Bxvd_Elf32_Half e_shstrndx;
} Bxvd_Elf32_Ehdr;

        /* e_type definitions */
#define BXVD_ET_NONE         0       /* No file type         */
#define BXVD_ET_REL          1       /* Relocatable file     */
#define BXVD_ET_EXEC         2       /* Executable file      */
#define BXVD_ET_DYN          3       /* Shared object file   */
#define BXVD_ET_CORE         4       /* Core file            */
#define BXVD_ET_LOPROC       0xFF00  /* Processor-specific   */
#define BXVD_ET_HIPROC       0xFFFF  /* Processor-specific   */

        /* e_machine definitions */
#define BXVD_EM_NONE         0       /* No machine           */
#define BXVD_EM_M32          1       /* AT&T WE 32100        */
#define BXVD_EM_SPARC        2       /* SPARC                */
#define BXVD_EM_386          3       /* Intel 80386          */
#define BXVD_EM_68K          4       /* Motorola 68000       */
#define BXVD_EM_88K          5       /* Motorola 88000       */
#define BXVD_EM_860          7       /* Intel 80860          */
#define BXVD_EM_MIPS         8       /* MIPS RS3000          */
#define BXVD_EM_ARC          45      /* Argonaut Risc Core   */

        /* e_version definitions */
#define BXVD_EV_NONE         0       /* Invalid version      */
#define BXVD_EV_CURRENT      1       /* Current version      */

        /* e_ident[] fields */
#define BXVD_EI_MAG0         0       /* File identification  */
#define BXVD_EI_MAG1         1       /* File identification  */
#define BXVD_EI_MAG2         2       /* File identification  */
#define BXVD_EI_MAG3         3       /* File identification  */
#define BXVD_EI_CLASS        4       /* File class           */
#define BXVD_EI_DATA         5       /* Data encoding        */
#define BXVD_EI_VERSION      6       /* File version         */
#define BXVD_EI_PAD          7       /* Start of padding     */

#define BXVD_ELFMAG0         0x7F
#define BXVD_ELFMAG1         'E'
#define BXVD_ELFMAG2         'L'
#define BXVD_ELFMAG3         'F'

#define BXVD_ELFCLASSNONE    0       /* Invalid class        */
#define BXVD_ELFCLASS32      1       /* 32-bit objects       */
#define BXVD_ELFCLASS64      2       /* 64-bit objects       */

#define BXVD_ELFDATANONE     0       /* Invalid data encoding        */
#define BXVD_ELFDATA2LSB     1       /* Little-endian encoding       */
#define BXVD_ELFDATA2MSB     2       /* Big-endian encoding          */

#define BXVD_SHN_UNDEF     0
#define BXVD_SHN_LORESERVE 0xff00
#define BXVD_SHN_LOPROC    0xff00
#define BXVD_SHN_HIPROC    0xff1f
#define BXVD_SHN_ABS       0xfff1
#define BXVD_SHN_COMMON    0xfff2
#define BXVD_SHN_HIRESERVE 0xffff

typedef struct
{      
      Bxvd_Elf32_Word  sh_name;      /* Section name (string tbl index)      */
      Bxvd_Elf32_Word  sh_type;      /* Section type                         */
      Bxvd_Elf32_Word  sh_flags;     /* Section flags                        */
      Bxvd_Elf32_Addr  sh_addr;      /* Section virtual addr at execution    */
      Bxvd_Elf32_Off   sh_offset;    /* Section file offset                  */
      Bxvd_Elf32_Word  sh_size;      /* Section size in bytes                */
      Bxvd_Elf32_Word  sh_link;      /* Link to another section              */
      Bxvd_Elf32_Word  sh_info;      /* Additional section information       */
      Bxvd_Elf32_Word  sh_addralign; /* Section alignment                    */
      Bxvd_Elf32_Word  sh_entsize;   /* Entry size if section holds table    */
} Bxvd_Elf32_Shdr;

#define BXVD_SHT_NULL        0
#define BXVD_SHT_PROGBITS    1
#define BXVD_SHT_SYMTAB      2
#define BXVD_SHT_STRTAB      3
#define BXVD_SHT_RELA        4
#define BXVD_SHT_HASH        5
#define BXVD_SHT_DYNAMIC     6
#define BXVD_SHT_NOTE        7
#define BXVD_SHT_NOBITS      8
#define BXVD_SHT_REL         9
#define BXVD_SHT_SHLIB       10
#define BXVD_SHT_DYNSYM      11
#define BXVD_SHT_LOPROC      0x70000000
#define BXVD_SHT_HIPROC      0x7FFFFFFF
#define BXVD_SHT_LOUSER      0x80000000
#define BXVD_SHT_HIUSER      0xFFFFFFFF

#define BXVD_SHF_WRITE       0x1
#define BXVD_SHF_ALLOC       0x2
#define BXVD_SHF_EXECINSTR   0x4
#define BXVD_SHF_MASKPROC    0xF0000000


typedef struct
{ 
      Bxvd_Elf32_Word st_name;
      Bxvd_Elf32_Addr st_value;
      Bxvd_Elf32_Word st_size;
      unsigned char   st_info;
      unsigned char   st_other;
      Bxvd_Elf32_Half st_shndx;
} Bxvd_Elf32_Sym;

#define BXVD_ELF32_ST_BIND(i)   ((i) >> 4)
#define BXVD_ELF32_ST_TYPE(i)   ((i) & 0x0F)
#define BXVD_ELF32_ST_INFO(b,t) (((b) << 4) + ((t) & 0x0F))

#define BXVD_STB_LOCAL       0
#define BXVD_STB_GLOBAL      1
#define BXVD_STB_WEAK        2
#define BXVD_STB_LOPROC      13
#define BXVD_STB_HIPROC      15

#define BXVD_STT_NOTYPE      0
#define BXVD_STT_OBJECT      1
#define BXVD_STT_FUNC        2
#define BXVD_STT_SECTION     3
#define BXVD_STT_FILE        4
#define BXVD_STT_LOPROC      13
#define BXVD_STT_HIPROC      15

typedef struct
{ 
      Bxvd_Elf32_Addr r_offset;
      Bxvd_Elf32_Word r_info;
} Bxvd_Elf32_Rel;

typedef struct
{
      Bxvd_Elf32_Addr  r_offset;
      Bxvd_Elf32_Word  r_info;
      Bxvd_Elf32_Sword r_addend;
} Bxvd_Elf32_Rela;

#define BXVD_ELF32_R_SYM(i)          ((i) >> 8)
#define BXVD_ELF32_R_TYPE(i)         ((unsigned char)(i))
#define BXVD_ELF32_R_INFO(s,t)       (((s) << 8) + (unsigned char)(t))


typedef struct
{ 
      Bxvd_Elf32_Word p_type;
      Bxvd_Elf32_Off  p_offset;
      Bxvd_Elf32_Addr p_vaddr;
      Bxvd_Elf32_Addr p_paddr;
      Bxvd_Elf32_Word p_filesz;
      Bxvd_Elf32_Word p_memsz;
      Bxvd_Elf32_Word p_flags;
      Bxvd_Elf32_Word p_align;
} Bxvd_Elf32_Phdr;

#define BXVD_PT_NULL         0
#define BXVD_PT_LOAD         1
#define BXVD_PT_DYNAMIC      2
#define BXVD_PT_INTERP       3
#define BXVD_PT_NOTE         4
#define BXVD_PT_SHLIB        5
#define BXVD_PT_PHDR         6
#define BXVD_PT_LOPROC       0x70000000
#define BXVD_PT_HIPROC       0x7FFFFFFF

/*----------------------------------------------------------------------*
 *                                                                      *
 * The following is the elf_arc.h file from Metaware.                   *
 *                                                                      *
 *----------------------------------------------------------------------*/


/*********************************************************************
 * THE SOFTWARE CONTAINED IN THIS FILE IS LICENSED TO BROADCOMS 
 * CUSTOMERS FOR USE SOLELY IN CONNECTION WITH ARC processors
 * within BROADCOM'S PRODUCTS.
 * (C) Copyright 2005; ARC International (ARC);  Santa Cruz, CA 95060
 * This program is the unpublished property and trade secret of ARC.  It
 * is to be  utilized  solely  under  license and it is to be maintained 
 * on a confidential basis. The security  and  protection  of  the program
 * is paramount to maintenance of the trade secret status.  It is to be 
 * protected from disclosure to unauthorized parties, both within the 
 * Licensee company and outside, in a manner not less stringent than that
 * utilized for Licensee's own proprietary internal information.  
 *********************************************************************/

/**********************************************************************
* Relocation types for the ARC ELF object files
*   A = addend used to compute the relocation
*   S = The value of the symbol being relocated
*   P = The place (addr/section offset) of the storage unit being relocated
*       (computed using r_offset)
*
* Relocation Fields (note this is big-endian notation...)
*   +--------------------------------+
*   |31          bits31-0           0| word32 data in separate word
*   +--------------------------------+
*
*   +------------------------+
*   |23      bits23-0       0| bits24 data in separate 3-byte chunk
*   +------------------------+
*
*   +----------------+
*   |15  bits15-0   0| bits16 data in separate half-word
*   +----------------+
*
*   +-----------+
*   |7 bits7-0 0| bits8 data in separate byte
*   +-----------+
*
*   +--------------------------------+ 
*   |31     |23     bits25-2        0| targ26 data as part of an instruction
*   +--------------------------------+
*
*   +--------------------------------+
*   |31  |26    bits21-2     7|     0| disp22 data as part of an instruction
*   +--------------------------------+
*
*   +--------------------------------------------+
*   |31  |26 bits10-1  17| |15 bits20-11 6|     0| disp21h data in instruction
*   +--------------------------------------------+ (half-word aligned)
*
*   +--------------------------------------------+
*   |31  |26 bits10-2  18| |15 bits20-11 6|     0| disp21w data in instruction
*   +--------------------------------------------+ (long-word aligned)
*
*   +------------------------------------------------------+
*   |31  |26 bits10-1  17| |15 bits20-11 6|  |3 bits24-21 0| disp25h data
*   +------------------------------------------------------+ (halfword aligned)
*
*   +------------------------------------------------------+
*   |31  |26 bits10-2  18| |15 bits20-11 6|  |3 bits24-21 0| disp25w data
*   +------------------------------------------------------+ (longword aligned)
*
*   +--------------------------------+
*   |31                  9| bits8-0 0| disp9 as part of an instruction
*   +--------------------------------+
*
*   +--------------------------------+
*   |31 |23 bits7-0 16|15 bit8|14   0| disp9ls as part of an instruction
*   +--------------------------------+
*
*   +-------------------+
*   |15     9| bits8-0 0| disp9s as part of an instruction
*   +-------------------+
*
*   +----------------==-+
*   |15   11| bits10-0 0| disp13s as part of an instruction (longword aligned)
*   +----------------==-+
*
*********************************************************************/

/*      Relo Type Name        Value    Field    Calculation */
#define R_ARC_NONE            0x0   /* none     None */
#define R_ARC_8               0x1   /* bits8    S + A */
#define R_ARC_16              0x2   /* bits16   S + A */
#define R_ARC_24              0x3   /* bits24   S + A */
#define R_ARC_32              0x4   /* word32   S + A */
#define R_ARC_B26             0x5   /* targ26   (S + A) >> 2    
                                     *(convert to longword displacement) 
                                     */
#define R_ARC_B22_PCREL       0x6   /* disp22   (S + A - P) >> 2
				     *    (convert to longword displacement)
                                     */
#define R_ARC_H30             0x7   /* word32   (S + A) >> 2 */
#define R_ARC_N8              0x8   /* bits8    S - A */
#define R_ARC_N16             0x9   /* bits16   S - A */
#define R_ARC_N24             0xA   /* bits24   S - A */
#define R_ARC_N32             0xB   /* word32   S - A */
#define R_ARC_SDA             0xC   /* disp9    S + A */
#define R_ARC_SECTOFF         0xD   /* word32   (S - <start of section>) + A */

/*************************************************************************
 * following new relocations defined for the ARCompact ISA
 *************************************************************************/

/* for conditional branch. Example: bne   printf */
#define R_ARC_S21H_PCREL  0xE   /* disp21h  (S + A - P) >> 1 
                                 * (convert to halfword displacement) 
                                 */

/* for conditional branch and link. Example: blne  printf */
#define R_ARC_S21W_PCREL  0xF   /* disp21w  (S + A - P) >> 2
                                 * (convert to longword displacement) 
                                 */

/* For unconditional branch.  Example: b  printf */
#define R_ARC_S25H_PCREL  0x10   /* disp25h  (S + A - P) >> 1 
                                  * (convert to halfword displacement)
                                  */

/* For unconditional branch and link.  Example: bl  printf */
#define R_ARC_S25W_PCREL  0x11   /* disp25w  (S + A - P) >> 2
                                  * (convert to longword displacement)
                                  */

/* for 32-bit Small Data Area fixups.  Example: add   r0, gp, var@sda */
#define R_ARC_SDA32       0x12   /* word32   (S + A) - _SDA_BASE_ */

/* for small data fixups on loads and stores.  Examples:
 *     ldb   r0,  [gp, var@sda]   ; R_ARC_SDA_LDST
 *     stw   r0,  [gp, var@sda]   ; R_ARC_SDA_LDST1
 *     ld    r0,  [gp, var@sda]   ; R_ARC_SDA_LDST2
 */
#define R_ARC_SDA_LDST   0x13   /* disp9ls (S + A - _SDA_BASE_)  (s9 range) */
#define R_ARC_SDA_LDST1  0x14   /* disp9ls (S+A-_SDA_BASE_) >> 1 (s10 range) */
#define R_ARC_SDA_LDST2  0x15   /* disp9ls (S+A-_SDA_BASE_) >> 2 (s11 range) */

/* for 16-bit load gp-relative instruction. Example:
 *     ldb_s  r0, [gp, var@sda]   ; R_ARC_SDA16_LD
 *     ldw_s  r0, [gp, var@sda]   ; R_ARC_SDA16_LD1
 *     ld_s   r0, [gp, var@sda]   ; R_ARC_SDA16_LD2
 */
#define R_ARC_SDA16_LD   0x16   /* disp9s  (S + A - _SDA_BASE)   (s9 range) */
#define R_ARC_SDA16_LD1  0x17   /* disp9s  (S+A-_SDA_BASE_) >> 1 (s10 range) */
#define R_ARC_SDA16_LD2  0x18   /* disp9s  (S+A-_SDA_BASE_) >> 2 (s11 range) */

/* for 16-bit branch-and-link.  Example: bl_s  printf */
#define R_ARC_S13_PCREL      0x19   /* disp13s  (S + A - P) >> 2 */

/* for 32-bit alignment of the fixup value.  Examples:
 *     mov   r0,  var@l
 *     ld    r0, [pcl, lab - .@l]
 */
#define R_ARC_W              0x1a   /* word32   (S + A) & ~3   (word-align) */

/*
 * The following relocations are to support middle-endian storage, whereby
 * a 32-bit word is stored in two halfwords, with bits 31-16 stored first
 * in memory and bits 15-0 stored adjacently.  The individual half-words are
 * stored in the native endian of the machine.  This is how all instructions
 * and LIMMs are stored in the ARCompact architecture.
 */
#define R_ARC_32_ME          0x1b   /* Like ARC_32, but stored in ME format */
#define R_ARC_N32_ME         0x1c   /* Like N32, but stored in ME format */
#define R_ARC_SECTOFF_ME     0x1d   /* Like SECTOFF, but stored in ME format */
#define R_ARC_SDA32_ME       0x1e   /* Like SDA32, but stored in ME format */
#define R_ARC_W_ME           0x1f   /* Like W, but stored in ME format */
#define R_ARC_H30_ME         0x20   /* Like H30, but stored in ME format */

/*************************************************************************/

/*
 * for ARC4 ld/st instructions, allows a section-relative offset in the
 * range 0-255 (the positive portion of the shimm range).  The base
 * register must be loaded with the base address of the section. Example:
 * ld   r0, [r20, var@sectoff_u8] 
 */

#define R_ARC_SECTOFF_U8     0x21   / disp9 (S + A - <start of section>) */

/* ARC4. range -256 to 255.  The base register must be loaded with base of
 * section + 256. Example:
 *   ld   r0, [r20, var@sectoff_s9] 
 */
#define R_ARC_SECTOFF_S9  0x22   /* disp9 (S + A - <start of section> - 256) */

/*
 * Same semantics as R_ARC_SECTOFF_U8 above, but for ARCompact ISA
 * Note, however that the assembler encodes these loads and stores with
 * address scaling (.as) turned on so that the range for half-words is
 * 0-510 and the range for full 32-bit word accesses is 0-1020
 * The range for byte accesses remains 0-255
 *    ldb  r0, [r20, var@sectoff_u8]   ; R_AC_SECTOFF_U8
 *    stw  r0, [r20, var@sectoff_u8]   ; R_AC_SECTOFF_U8_1
 *    ld   r0, [r20, var@sectoff_u8]   ; R_AC_SECTOFF_U8_2
 */
#define R_AC_SECTOFF_U8   0x23   /* disp9ls (S + A - <start of section>) */
#define R_AC_SECTOFF_U8_1 0x24   /* disp9ls (S +A - <start of section>) >> 1 */
#define R_AC_SECTOFF_U8_2 0x25   /* disp9ls (S +A - <start of section>) >> 2 */

/*
 * Same semantics as R_ARC_SECTOFF_S9 above, but for ARCompact ISA
 * Note, however that the assembler encodes these loads and stores with
 * address scaling (.as) turned on so that the range for half-words is
 * -256-510 and the range for full 32-bit word accesses is -256-1020
 * The range for byte accesses remains -256-255
 *    ldb  r0, [r20, var@sectoff_s9]   ; R_AC_SECTOFF_S9
 *    stw  r0, [r20, var@sectoff_s9]   ; R_AC_SECTOFF_S9_1
 *    ld   r0, [r20, var@sectoff_s9]   ; R_AC_SECTOFF_S9_2
 */
#define R_AC_SECTOFF_S9   0x26   /* disp9ls (S + A - <start of section>) */
#define R_AC_SECTOFF_S9_1 0x27   /* disp9ls (S +A - <start of section>) >> 1 */
#define R_AC_SECTOFF_S9_2 0x28   /* disp9ls (S +A - <start of section>) >> 2 */

/* X/Y memory relocations */
#define R_ARC_SECTOFF_ME_1   0x29   /* word32  ((S - <start of section>) + A)
                                     * >> 1 [ME format]
                                     */

#define R_ARC_SECTOFF_ME_2   0x2a   /* word32  ((S - <start of section>) + A)
                                     * >> 2 [ME format]
                                     */

#define R_ARC_SECTOFF_1      0x2b   /* word32  ((S - <start of section>) + A)
                                     * >> 1
                                     */

#define R_ARC_SECTOFF_2      0x2c   /* word32  ((S - <start of section>) + A)
                                     * >> 2
                                     */

#endif
