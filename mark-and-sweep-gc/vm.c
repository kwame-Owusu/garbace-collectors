#include "vm.h"

vm_t *vm_new() {
  vm_t *vm_alloc = malloc(sizeof(vm_t)); 
  if(vm_alloc == NULL){
    return NULL;
  }
  stack_t *frames = stack_new(8);
  stack_t *objects = stack_new(8);
  if(frames == NULL){
    free(vm_alloc);
    return NULL;
  }
  if(objects == NULL){
    stack_free(frames);
    free(vm_alloc);
    return NULL;
  }
  vm_alloc->frames = frames;
  vm_alloc->objects= objects;
  return vm_alloc;
}

void vm_free(vm_t *vm) {
 stack_free(vm->frames);
 stack_free(vm->objects);
 stack_free(vm);
}
