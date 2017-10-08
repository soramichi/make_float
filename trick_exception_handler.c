// build with -lm (?????)

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fenv.h>
#include <ucontext.h>

static void __attribute__ ((constructor)) trapfpe () {
  // Enable some exceptions.  At startup all exceptions are masked. 
  feenableexcept (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
}

// Relevant resources
//    ucontext_t: https://linuxjm.osdn.jp/html/LDP_man-pages/man3/getcontext.3.html
//    mcontext_t: https://gcc.gnu.org/bugzilla/attachment.cgi?id=21104
//    gregset_t: http://alien.cern.ch/cache/glibc-2.3.2/sysdeps/unix/sysv/linux/sparc/sys/ucontext.h
//    fpregset_t: https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/x86/sys/ucontext.h.html#_libc_fpstate
void handler(int signum, siginfo_t* info, void* _context) {
  ucontext_t* context = (ucontext_t*)_context;  

  printf("Received signal: %d\n", signum);
  // access a general purpose register
  printf("rip: %p\n", context->uc_mcontext.gregs[REG_RIP]);
  // access an SSE register
  printf("xmm0: 0x%x\n", context->uc_mcontext.fpregs->_xmm[0]);

  // Usage:
  //     validate only one from Pattern 1 - 3, and comment-out the others
  
  // Pattern 1:
  //     Jump back to the 'next' instruction of 'addss  %xmm1,%xmm0'.
  //     Because xmm = 1.0 right now and xmm0 is stored to 'a' by 'movss  %xmm0,-0x8(%rbp)',
  //     'a' is 1.0 when it is printed.
  context->uc_mcontext.gregs[REG_RIP] = context->uc_mcontext.gregs[REG_RIP] + 4;

  // Pattern 2:
  //     Jump back to the 'next next' instruction of "addss  %xmm1,%xmm0".
  //     Because 'movss  %xmm0,-0x8(%rbp)' is skipped and 'a' is not updated,
  //     'a' is 2.0 when it is printed.
  // context->uc_mcontext.gregs[REG_RIP] = context->uc_mcontext.gregs[REG_RIP] + 9;

  // Pattern 3:
  //     Set xmm0 = 3.0 and jump back to the 'next' instruction of 'addss  %xmm1,%xmm0'.
  //     Because xmm0 is stored to 'a' by 'movss  %xmm0,-0x8(%rbp)',
  //     'a' is 3.0 when it is printed.
  // context->uc_mcontext.fpregs->_xmm[0].element[0] = 0x40400000; // 3.0
  // context->uc_mcontext.gregs[REG_RIP] = context->uc_mcontext.gregs[REG_RIP] + 4;
  
  return;
}


int main(){
  unsigned int b = 0x7f803039;
  float nan = *((float*)(&b));
  float a = 2.0;

  // register a signal handler
  struct sigaction action;
  action.sa_sigaction = handler;
  action.sa_flags = SA_SIGINFO;
  sigaction(SIGFPE, &action, NULL);

  // exception
  //   f3 0f 10 4d fc          movss  -0x4(%rbp),%xmm1   # xmm1 = nan
  //   f3 0f 10 05 f9 00 00    movss  0xf9(%rip),%xmm0   # xmm0 = 1.0
  //   00
  //   f3 0f 58 c1             addss  %xmm1,%xmm0        # xmm0 = xmm1 + xmm0
  //   f3 0f 11 45 f8          movss  %xmm0,-0x8(%rbp)   # a = xmm0
  a = nan + 1.0;

  printf("the end of main (%f)\n", a);

  return 0;
}
