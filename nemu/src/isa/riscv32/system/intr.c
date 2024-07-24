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
#include <../local-include/reg.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  csr(MEPC) = epc;
  csr(MCAUSE) = NO;
  return csr(MTVEC);
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}

word_t etrace_isa_raise_intr(word_t NO, vaddr_t epc) {
  word_t ret = isa_raise_intr(NO, epc);
#ifdef CONFIG_ETRACE
  printf("ETRACE: MCAUSE["
  ANSI_FMT(FMT_WORD, ANSI_FG_BLUE) "], MEPC["
  ANSI_FMT(FMT_WORD, ANSI_FG_BLUE) "], MSTATUS["
  ANSI_FMT(FMT_WORD, ANSI_FG_BLUE) "], MTVEC["
  ANSI_FMT(FMT_PADDR, ANSI_FG_BLUE) "].\n",
  csr(MCAUSE), csr(MEPC), csr(MSTATUS), csr(MTVEC));
#endif
  return ret;
}
