#include <csos/syscall.h>

void psleep(uint32_t ms){
    sleep(ms);
}

int vsprintf2(char *buf, const char *fmt, va_list args);
// static inline int printf(const char *fmt, ...) __attribute__((always_inline));
int printf2(const char *fmt, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    int i = vsprintf2(buffer, fmt, args);
    va_end(args);
    syscall_arg_t printf_arg = { SYS_NR_PRINTF, (uint32_t)buffer, i, 0, 0 };
    return _syscall(&printf_arg);
}



//  static inline int printf(const char *fmt, ...){
// printf(fmt,)
//  }
// printkf()

// static void psleep(){
//     sleep()
// }