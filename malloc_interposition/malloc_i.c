#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <stdint.h>

// This should only work for single-threaded processes

size_t mem_total = 0;

void * addr;

void exit_handler(int a, ...)
{
  printf("Called for: %u bytes", (unsigned int) mem_total);
}

void *malloc(size_t size)
{
  static void * (*func)();

  mem_total += size;

  if(!func)
  {
    func = (void *(*)()) dlsym(RTLD_NEXT, "malloc");
    addr = func(1024*1024*100);
    if(addr)
    {
      printf("Allocation was succesful\n");
      printf("The heap starts at 0x%lx\n", (unsigned long int) addr);
    }
    else
      exit(-100);
      on_exit(exit_handler, 0);
    }

  void * old_addr = addr;
  addr += size;
  //printf("0x%x - 0x%x\n", old_addr, size);
  return old_addr;
}

void * free(void *ptr)
{
  static void *(*func_free)();

  if(!func_free)
    dlsym(RTLD_NEXT, "free");
}

void * realloc(void *ptr, size_t size)
{
  //fprintf(stderr, "0x%x - 0x%d\n", ptr, size);
  static void *(*func_realloc)();
  if(!func_realloc)
    dlsym(RTLD_NEXT, "realloc");

  if(ptr)
  {
    void * c_ptr = malloc(size);

    //printf("0x%x - 0x%x - 0x%x\n", c_ptr, ptr, size);
    memmove(c_ptr, ptr, size); // abuse the size, but be sure to copy everything 

    return c_ptr;
  }
  else
  {
    return malloc(size);
  }
}

