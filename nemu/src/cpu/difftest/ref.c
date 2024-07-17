/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <difftest-def.h>
#include <memory/paddr.h>

#define NR_GPR MUXDEF(CONFIG_RVE, 16, 32)

__EXPORT void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
  uint8_t *src, *dst;
  if(direction == DIFFTEST_TO_DUT) {
    src = guest_to_host(addr);
    dst = buf;
  }
  else {
    src = buf;
    dst = guest_to_host(addr);
  }
  memcpy(dst, src, n);
}

__EXPORT void difftest_regcpy(void *dut, bool direction) {
  CPU_state *src, *dst;
  if(direction == DIFFTEST_TO_DUT) {
    src = &cpu;
    dst = dut;
  }
  else {
    src = dut;
    dst = &cpu;
  }
  for (int i = 0; i < NR_GPR; i++) {
    dst->gpr[i] = src->gpr[i];
  }
}

__EXPORT void difftest_exec(uint64_t n) {
  cpu_exec(n);
}

__EXPORT void difftest_raise_intr(word_t NO) {
  assert(0);
}

__EXPORT void difftest_init(int port) {
  void init_mem();
  init_mem();
  /* Perform ISA dependent initialization. */
  init_isa();
}
