#include <unistd.h>
#include <sys/syscall.h>
#include <stddef.h>

/**********************************************************************************
** arch	syscall NR	return	arg0	arg1	arg2	arg3	arg4	arg5
** arm	r7	r0	r0	r1	r2	r3	r4	r5
** arm64	x8	x0	x0	x1	x2	x3	x4	x5
** x86	eax	eax	ebx	ecx	edx	esi	edi	ebp
** x86_64	rax	rax	rdi	rsi	rdx	r10	r8	r9
**********************************************************************************/

ssize_t my_write(int fd, const void *buf, ssize_t count) {

#if defined(__x86_64__)
    // x86_64 Linux syscall convention
    register long rax __asm__("rax") = SYS_write;
    register long rdi __asm__("rdi") = fd;
    register const void *rsi __asm__("rsi") = buf;
    register long rdx __asm__("rdx") = count;

    __asm__ volatile(
        "syscall"
        : "+r"(rax)
        : "r"(rdi), "r"(rsi), "r"(rdx)
        : "rcx", "r11", "memory"
    );
    return (ssize_t)rax;

#elif defined(__i386__)
    // x86 32-bit Linux syscall convention
    register long eax __asm__("eax") = SYS_write;
    register long ebx __asm__("ebx") = fd;
    register const void *ecx __asm__("ecx") = buf;
    register long edx __asm__("edx") = count;

    __asm__ volatile(
        "int $0x80"
        : "+r"(eax)
        : "r"(ebx), "r"(ecx), "r"(edx)
        : "memory"
    );
    return (ssize_t)eax;

#elif defined(__arm__)
    // ARM 32-bit Linux syscall convention
    register long r7 __asm__("r7") = SYS_write;
    register long r0 __asm__("r0") = fd;
    register const void *r1 __asm__("r1") = buf;
    register long r2 __asm__("r2") = count;

    __asm__ volatile(
        "swi 0"
        : "+r"(r0)
        : "r"(r7), "r"(r1), "r"(r2)
        : "memory"
    );
    return (ssize_t)r0;

#elif defined(__aarch64__)
    // ARM64 Linux syscall convention
    register long x8 __asm__("x8") = SYS_write;
    register long x0 __asm__("x0") = fd;
    register const void *x1 __asm__("x1") = buf;
    register long x2 __asm__("x2") = count;

    __asm__ volatile(
        "svc #0"
        : "+r"(x0)
        : "r"(x8), "r"(x1), "r"(x2)
        : "memory"
    );
    return (ssize_t)x0;

#else
#error "Unsupported architecture"
#endif

}

ssize_t myPrintf(const void *buf){
   
   ssize_t retLen = 0;
   // check on null pointers
   if(NULL == buf){
      retLen = -1;
   }else{
      ssize_t size = 0;

      while('\0' != ((char *)buf)[size]){
          size++;
      }

      retLen = my_write(1,buf,size);
   }

   return(retLen);
}
