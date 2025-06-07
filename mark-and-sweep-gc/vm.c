#include "vm.h"
#include "stack.h"
#include "snekobject.h"


void trace(vm_t *vm) {
  stack_t *gray_objects = stack_new(8);
  if(gray_objects == NULL){
    return;
  }

  for(size_t i = 0; i < vm->objects->count; i++){
    snek_object_t *obj = vm->objects->data[i];
    if(obj->is_marked){
      stack_push(gray_objects, obj);
    }
  }
  
  while(gray_objects->count != 0){
    void* obj_pop = stack_pop(gray_objects);
    trace_blacken_object(gray_objects,obj_pop);
  }

  stack_free(gray_objects);
}
  


void trace_blacken_object(stack_t *gray_objects, snek_object_t *obj) {
  if(obj->kind == INTEGER || obj->kind == FLOAT ||obj->kind == STRING){
   return; 
  }
  if(obj->kind == VECTOR3){
    trace_mark_object(gray_objects, obj->data.v_vector3.x);
    trace_mark_object(gray_objects, obj->data.v_vector3.y);
    trace_mark_object(gray_objects, obj->data.v_vector3.z);
  }
  if(obj->kind == ARRAY){
    snek_array_t array =obj->data.v_array;
    for(size_t i = 0; i < array.size; i++){
      trace_mark_object(gray_objects, array.elements[i]);
    }
  }
}

void trace_mark_object(stack_t *gray_objects, snek_object_t *obj) {
  if(obj == NULL || obj->is_marked){
   return;
  }
  stack_push(gray_objects, obj);
  obj->is_marked = true;
  
}

void mark(vm_t *vm) {
  for(int i = 0; i < vm->frames->count; i++){
    frame_t *temp = (frame_t *)vm->frames->data[i];
    for (int j = 0; j < temp->references->count ; j++){
      snek_object_t *obj = (snek_object_t *)temp->references->data[j];
      obj->is_marked = true;
    }
  }
}


void frame_reference_object(frame_t *frame, snek_object_t *obj) {
  stack_push(frame->references, obj); 
}

void vm_free(vm_t *vm) {
  for(int i = 0; i < vm->frames->count; i++){
    frame_free((snek_object_t *)vm->frames->data[i]);
  }
  stack_free(vm->frames);
  
  for(int i = 0; i < vm->objects->count; i++){
    snek_object_free((snek_object_t *)vm->objects->data[i]);
  }
  stack_free(vm->objects);
  free(vm);
}


vm_t *vm_new() {
  vm_t *vm = malloc(sizeof(vm_t));
  if (vm == NULL) {
    return NULL;
  }

  vm->frames = stack_new(8);
  vm->objects = stack_new(8);
  return vm;
}

void vm_track_object(vm_t *vm, snek_object_t *obj) {
  stack_push(vm->objects, obj);
}

void vm_frame_push(vm_t *vm, frame_t *frame) {
  stack_push(vm->frames, frame);
}

frame_t *vm_new_frame(vm_t *vm) {
  frame_t *frame = malloc(sizeof(frame_t));
  frame->references = stack_new(8);

  vm_frame_push(vm, frame);
  return frame;
}

void frame_free(frame_t *frame) {
  stack_free(frame->references);
  free(frame);
}
