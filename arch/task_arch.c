/*
 * Copyright (c) 2023 Qoda, engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms and conditions of the GNU General Public License,
 * version 3 or later, as published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received copies of the GNU General Public License and
 * the GNU Lesser General Public License along with this program.  If
 * not, see https://www.gnu.org/licenses/
 */
#include "common.h"
#include "processor.h"
#include "task.h"

/******************************************************************************
 * @brief initialize task stack
 *
 * The stack follows the riscv ABI i.e task_rt() argument is stored in a0 and sp
 * is 128 bits aligned:
 *
 * ra
 * sp -> stack_start + stack_size - 128
 * s0
 * s1
 * s2
 * s3
 * s4
 * s5
 * s6
 * s7
 * s8
 * s9
 * s10
 * s11
 * a0 -> task_entry
 * a1
 * a2
 * a3
 * a4
 * a5
 * a6
 * a7
 * ...
 * task_rt
 *
 * @param stack start address pointer
 * @param size of the stack
 * @param function to run in the task context
 * @return top address of the initialized stack
 ******************************************************************************/
void task_stack_init(stack_t *stack, uint64_t stack_size,
                     void (*task_entry)(void)) {
  // get task pointer from the stack
  task_t *task = (task_t *)stack;

  // zeroied callee-saved registers
  task->thread.s[0]  = 0;
  task->thread.s[1]  = 0;
  task->thread.s[2]  = 0;
  task->thread.s[3]  = 0;
  task->thread.s[4]  = 0;
  task->thread.s[5]  = 0;
  task->thread.s[6]  = 0;
  task->thread.s[7]  = 0;
  task->thread.s[8]  = 0;
  task->thread.s[9]  = 0;
  task->thread.s[10] = 0;
  task->thread.s[11] = 0;

  // save task entry in the a0 register which is the first function parameter in
  // the riscv ABI
  task->thread.a[0] = (uint64_t)task_entry;

  // save the return address at the first address of the stack
  uint64_t stack_return_addr       = (uint64_t)stack + stack_size - DWORD_SIZE;
  *(uint64_t *)(stack_return_addr) = (uint64_t)task_rt;

  // move SP (128 bits aligned) to save return address
  task->thread.sp = (uint64_t)stack + STACK_SIZE - LWORD_SIZE;
}