/*************************************************************************
	> File Name: elf.c
	> Author: xh	
	> Mail: xh13242981739@163.com 
	> Created Time: 2024年07月15日 星期一 16时38分38秒
 ************************************************************************/

#include <common.h>
#include <elf.h>

#define MAX_FUC 2000
#define MAX_BT 10

typedef struct fuction_address{
	vaddr_t start;
	vaddr_t end;
	char fuc_name[64];
	// FUC *next;
} FUC;

static FILE *elf_fp = NULL;
static FUC fuc[MAX_FUC];
static int fuc_num = 0;
static int depth = 0;
// static FUC bt[MAX_BT];
// static int head;

void init_elf(int elf_num, char *elf_file[]) {
	if(elf_num == 0) {
		return;
	}
	for (int i = 0; i < elf_num; i++) {
		elf_fp = fopen(elf_file[i], "r");
		Assert(elf_fp, "Can not open '%s'", elf_file[i]);
		printf("open %s\n", elf_file[i]);
		Elf32_Ehdr elf_header;
		int ret = fread(&elf_header, sizeof(Elf32_Ehdr), 1, elf_fp);
		assert(ret == 1);
		// for (int i = 0; i < 16;i++) {
		// 	printf("%x  ", elf_header.e_ident[i]);
		// }
		ret = fseek(elf_fp, elf_header.e_shoff, SEEK_SET);
		assert(ret == 0);
		Elf32_Shdr symtab;
		for (int i = 0; i < elf_header.e_shnum; i++) {
			ret = fread(&symtab, elf_header.e_shentsize, 1, elf_fp);
			assert(ret == 1);
			if(symtab.sh_type == SHT_SYMTAB) {break;}
			assert(i != elf_header.e_shnum);
		}

		ret = fseek(elf_fp, elf_header.e_shoff + elf_header.e_shentsize * symtab.sh_link, SEEK_SET);
		assert(ret == 0);
		Elf32_Shdr strtab_head;
		ret = fread(&strtab_head, elf_header.e_shentsize, 1, elf_fp);
		assert(ret == 1);
		char strtab[strtab_head.sh_size];
		ret = fseek(elf_fp, strtab_head.sh_offset, SEEK_SET);
		assert(ret == 0);
		ret = fread(strtab, strtab_head.sh_size, 1, elf_fp);
		assert(ret == 1);

		ret = fseek(elf_fp, symtab.sh_offset, SEEK_SET);
		assert(ret == 0);
		Elf32_Sym symtab_entry;
		for (int i = 0; i < symtab.sh_size / symtab.sh_entsize; i++) {
			ret = fread(&symtab_entry, symtab.sh_entsize, 1, elf_fp);
			assert(ret == 1);
			if(ELF32_ST_TYPE(symtab_entry.st_info) == STT_FUNC) {
				fuc[fuc_num].start = symtab_entry.st_value;
				fuc[fuc_num].end = fuc[fuc_num].start + symtab_entry.st_size;
				char *name = strtab + symtab_entry.st_name;
				strcpy(fuc[fuc_num].fuc_name, name);
				// printf("name:%s,  start:%x, end:%x\n", fuc[fuc_num].fuc_name, fuc[fuc_num].start, fuc[fuc_num].end);
				fuc_num++;
				Assert(fuc_num <= MAX_FUC, "Too many fuctions.");
			}
		}
		fclose(elf_fp);
	} 
}

int which_fuc(vaddr_t pc) {
	for (int i = 0; i < fuc_num; i++) {
		if(pc >= fuc[i].start && pc < fuc[i].end) {
			return i;
		}
	}
	return -1;
}

void fuc_trace(vaddr_t pc, vaddr_t dnpc, int type) {
	int idx1 = which_fuc(pc);
	if(dnpc >= fuc[idx1].start && dnpc < fuc[idx1].end) {
		return;
	}
	int idx2 = which_fuc(dnpc);

	printf(FMT_WORD":", pc);
	for (int i = 0; i < depth; i++) {
		printf("  ");
	}
	printf((type == 0) ? "  call[%s@0x" FMT_WORD "]\n" : "ret[%s@0x" FMT_WORD "]\n",
				 (idx2 == -1 ? "" : fuc[idx2].fuc_name), dnpc);
	depth = (type == 0) ? depth + 1 : depth - 1;
}

// void back_trace(vaddr_t pc, vaddr_t dnpc, int type) {
// 	if(type == 0) {

// 	}
// }

void curr_fuc(vaddr_t pc) {
	int idx1 = which_fuc(pc);
	printf(FMT_WORD":", pc);
	printf(" : %s\n", fuc[idx1].fuc_name);
}