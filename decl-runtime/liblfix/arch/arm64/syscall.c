// For mremap
#define _GNU_SOURCE

#include "syscall.h"
#include "sys.h"

static int
sysrenameat(LFIXProc* p, int oldfd, uintptr_t oldp, int newfd, uintptr_t newp)
{
    return sysrenameat2(p, oldfd, oldp, newfd, newp, 0);
}
SYSWRAP_4(sysrenameat, int, uintptr_t, int, uintptr_t);

enum {
    LSYS_getcwd          = 17,
    LSYS_fcntl           = 25,
    LSYS_ioctl           = 29,
    LSYS_unlinkat        = 35,
    LSYS_renameat        = 38,
    LSYS_faccessat       = 48,
    LSYS_chdir           = 49,
    LSYS_openat          = 56,
    LSYS_close           = 57,
    LSYS_pipe2           = 59,
    LSYS_getdents64      = 61,
    LSYS_lseek           = 62,
    LSYS_read            = 63,
    LSYS_write           = 64,
    LSYS_readv           = 65,
    LSYS_writev          = 66,
    LSYS_readlinkat      = 78,
    LSYS_newfstatat      = 79,
    LSYS_fstat           = 80,
    LSYS_exit            = 93,
    LSYS_exit_group      = 94,
    LSYS_set_tid_address = 96,
    LSYS_clock_gettime   = 113,
    LSYS_rt_sigprocmask  = 135,
    LSYS_getpid          = 172,
    LSYS_gettid          = 178,
    LSYS_sysinfo         = 179,
    LSYS_brk             = 214,
    LSYS_munmap          = 215,
    LSYS_mremap          = 216,
    LSYS_clone           = 220,
    LSYS_execve          = 221,
    LSYS_mmap            = 222,
    LSYS_wait4           = 260,
    LSYS_prlimit64       = 261,
    LSYS_getrandom       = 278,
    LSYS_rt_sigaction    = 134,
    LSYS_prctl           = 167,
    LSYS_madvise         = 233,
    LSYS_mprotect        = 226,
    LSYS_membarrier      = 283,
    LSYS_dup             = 23,
    LSYS_futex           = 98,
    LSYS_sched_getaffinity = 123,
};

SyscallFn syscalls[] = {
    [LSYS_read]              = sysread_,
    [LSYS_write]             = syswrite_,
    [LSYS_readv]             = sysreadv_,
    [LSYS_writev]            = syswritev_,
    [LSYS_set_tid_address]   = sysignore_,
    [LSYS_brk]               = sysbrk_,
    [LSYS_ioctl]             = sysignore_,
    [LSYS_exit_group]        = sysexit_,
    [LSYS_exit]              = sysexit_,
    [LSYS_close]             = sysclose_,
    [LSYS_fstat]             = sysfstat_,
    [LSYS_newfstatat]        = sysfstatat_,
    [LSYS_openat]            = sysopenat_,
    [LSYS_mmap]              = sysmmap_,
    [LSYS_munmap]            = sysmunmap_,
    [LSYS_mprotect]          = sysmprotect_,
    [LSYS_fcntl]             = sysignore_,
    [LSYS_clock_gettime]     = sysclock_gettime_,
    [LSYS_rt_sigprocmask]    = sysignore_,
    [LSYS_getrandom]         = sysgetrandom_,
    [LSYS_getpid]            = sysgetpid_,
    [LSYS_mremap]            = syserror_,
    [LSYS_madvise]           = sysignore_,
    [LSYS_getcwd]            = sysgetcwd_,
    [LSYS_lseek]             = syslseek_,
    [LSYS_prctl]             = sysignore_,
    [LSYS_readlinkat]        = sysreadlinkat_,
    [LSYS_faccessat]         = sysfaccessat_,
    [LSYS_renameat]          = sysrenameat_,
    [LSYS_sysinfo]           = syssysinfo_,
    [LSYS_dup]               = sysdup_,
    [LSYS_getdents64]        = sysgetdents64_,
    [LSYS_prlimit64]         = sysignore_,
    [LSYS_chdir]             = syschdir_,
    [LSYS_unlinkat]          = sysunlinkat_,
    [LSYS_futex]             = sysfutex_,
    [LSYS_rt_sigaction]      = sysignore_,
    [LSYS_gettid]            = sysignore_,
    [LSYS_sched_getaffinity] = sysignore_,
};

_Static_assert(sizeof(syscalls) / sizeof(SyscallFn) < SYS_max, "syscalls exceed SYS_max");
