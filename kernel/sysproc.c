#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  struct proc *p = myproc();
  int ret = p->pid;
  if (p->strace_m & (1 << SYS_getpid))
    printf("%d: syscall getpid () -> %d\n", ret, ret);
  return ret;
}

uint64
sys_fork(void)
{
  int ret = fork();
  struct proc *p = myproc();
  if (p->strace_m & (1 << SYS_fork))
    printf("%d: syscall fork () -> %d\n", p->pid, ret);
  return ret;
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  int ret = wait(p);
  struct proc *myp = myproc();
  if (myp->strace_m & (1 << SYS_wait))
    printf("%d: syscall wait (%d) -> %d\n", myp->pid, p, ret);
  return ret;
}

uint64
sys_sigalarm(void)
{
  return 0;
}

uint64
sys_trace(void)
{
  uint64 addr;
  argaddr(0, &addr);
  int ret = trace(addr);
  return ret;
}
 
uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;
  struct proc *p = myproc();

  argint(0, &n);
  addr = p->sz;
  if(growproc(n) < 0)
    return -1;

  if (p->strace_m & (1 << SYS_sbrk))
    printf("%d: syscall sbrk (%d) -> %d\n", p->pid, n, addr);
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;
  struct proc *p = myproc();

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(p)){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  if (p->strace_m & (1 << SYS_sleep))
    printf("%d: syscall sleep (%u) -> 0\n", p->pid, ticks);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  int ret = kill(pid);
  struct proc *p = myproc();
  if (p->strace_m & (1 << SYS_kill))
    printf("%d: syscall kill (%d) -> %d\n", p->pid, pid, ret);
  return ret;
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  struct proc *p = myproc();
  if (p->strace_m & (1 << SYS_uptime))
    printf("%d: syscall uptime () -> %u\n", p->pid, xticks);
  return xticks;
}
