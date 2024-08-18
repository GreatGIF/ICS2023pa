#include <proc.h>
#include <elf.h>
#include <fs.h>

#define USTACK_PGNUM 8

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_X86__)
# define EXPECT_TYPE EM_386
#elif defined(__riscv)
# define EXPECT_TYPE EM_RISCV
#elif defined(__ISA_MIPS32_H__)
# define EXPECT_TYPE EM_MIPS
#else
# define EXPECT_TYPE -1
#endif

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  // Log("load %s.", filename);
  Elf_Ehdr elf_header;
  int fp = fs_open(filename, 0, 0);
  if(fp == -1) {return 0;}
  fs_read(fp, &elf_header, sizeof(Elf_Ehdr));
  // printf("fp:%d\n", fp);
  // ramdisk_read(&elf_header, 0, sizeof(Elf_Ehdr));

  assert(*(uint32_t *)elf_header.e_ident == 0x464c457f);
  assert(elf_header.e_machine == EXPECT_TYPE);

  Elf_Phdr program_header_tb[elf_header.e_phnum];
  fs_lseek(fp, elf_header.e_phoff, SEEK_SET);
  fs_read(fp, program_header_tb, elf_header.e_phnum * elf_header.e_phentsize);
  // ramdisk_read(program_header_tb, elf_header.e_phoff, elf_header.e_phnum * elf_header.e_phentsize);
  for(int i = 0; i < elf_header.e_phnum; i++) {
    // printf("i=%d, e_phnum=%d\n", i, elf_header.e_phnum);
    // printf("offset=%x\n", program_header_tb[i].p_offset);
    // printf("size=%x\n", program_header_tb[i].p_filesz);
    if(program_header_tb[i].p_type != PT_LOAD) {continue;}
    fs_lseek(fp, program_header_tb[i].p_offset, SEEK_SET);
    fs_read(fp, (void *)program_header_tb[i].p_vaddr, program_header_tb[i].p_filesz);
    // ramdisk_read((void *)program_header_tb[i].p_vaddr, program_header_tb[i].p_offset, program_header_tb[i].p_filesz);
    memset((void *)(program_header_tb[i].p_paddr + program_header_tb[i].p_filesz), 0, program_header_tb[i].p_memsz - program_header_tb[i].p_filesz);
  }

  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  int argv_size = 0, envp_size = 0;
  for(; argv[argv_size] != NULL; argv_size++);
  for(; envp[envp_size] != NULL; envp_size++);
  // Log("argv_size=%d, envp_size=%d", argv_size, envp_size);

  uint8_t *sp = (uint8_t *)new_page(USTACK_PGNUM) + PGSIZE * USTACK_PGNUM;

  for(int i = 0; i < argv_size; i++) {   
    int len = strlen(argv[i]) + 1;
    sp -= len;
    strncpy((char *)sp, argv[i], len);
  }
  for(int i = 0; i < envp_size; i++) {
    int len = strlen(envp[i]) + 1;
    sp -= len;
    strncpy((char *)sp, envp[i], len);
  }

  uint8_t *string_area = sp;
  int len = sizeof(uint8_t *);
  sp -= len;
  memset(sp, 0, len);
  for(int i = envp_size - 1; i >= 0; i--) {
    sp -= len;
    memcpy(sp, &string_area, len);
    string_area += strlen(envp[i]) + 1;
  }
  sp -= len;
  memset(sp, 0, len);
  for(int i = argv_size - 1; i >= 0; i--) {
    sp -= len;
    memcpy(sp, &string_area, len);
    string_area += strlen(argv[i]) + 1;
  }
  sp -= sizeof(int);
  memcpy(sp, &argv_size, sizeof(int));

  uintptr_t entry = loader(pcb, filename);
  pcb->cp = ucontext(NULL, (Area){pcb->stack, pcb + 1}, (void *)entry);
  pcb->cp->GPRx = (uintptr_t)sp;
  // Log("here");
}
