/*************************************************************************
	> File Name: elf.c
	> Author: xh	
	> Mail: xh13242981739@163.com 
	> Created Time: 2024年07月15日 星期一 16时38分38秒
 ************************************************************************/

#include <common.h>
#include <elf.h>

static FILE *elf_fp = NULL;

void init_elf(const char *elf_file) {
	if(elf_file == NULL) {
		return;
	}
	elf_fp = fopen(elf_file, "r");
	Assert(elf_fp, "Can not open '%s'", elf_file);
	Elf32_Ehdr elf_header;
	int ret = fread(&elf_header, sizeof(Elf32_Ehdr), 1, elf_fp);
	Assert(ret == 1, "Failed to read elf header.") ;
	// for (int i = 0; i < 16;i++) {
	// 	printf("%x  ", elf_header.e_ident[i]);
	// }
	printf("\n");
}
