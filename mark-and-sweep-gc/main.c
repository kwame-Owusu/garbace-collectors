#include <stdio.h>
#include <stdlib.h>

#include "bootlib.h"
#include "../lib/munit.h"
#include "sneknew.h"
#include "snekobject.h"
#include "vm.h"


munit_case(RUN, test_trace_vector, {
  vm_t *vm = vm_new();
  frame_t *frame = vm_new_frame(vm);

  snek_object_t *x = new_snek_integer(vm, 5);
  snek_object_t *y = new_snek_integer(vm, 5);
  snek_object_t *z = new_snek_integer(vm, 5);
  snek_object_t *vector = new_snek_vector3(vm, x, y, z);

  // nothing is marked
  assert_false(x->is_marked);
  assert_false(y->is_marked);
  assert_false(z->is_marked);
  assert_false(vector->is_marked);

  // After referencing and marking, the
  // vector should be marked, but not the contents
  frame_reference_object(frame, vector);
  mark(vm);
  assert_true(vector->is_marked);
  assert_false(x->is_marked);
  assert_false(y->is_marked);
  assert_false(z->is_marked);

  // After tracing, the contents should be marked
  trace(vm);
  assert_true(vector->is_marked);
  assert_true(x->is_marked);
  assert_true(y->is_marked);
  assert_true(z->is_marked);

  vm_free(vm);
  assert(boot_all_freed());
});

munit_case(SUBMIT, test_trace_array, {
  vm_t *vm = vm_new();
  frame_t *frame = vm_new_frame(vm);

  snek_object_t *devs = new_snek_array(vm, 2);
  snek_object_t *lane = new_snek_string(vm, "Lane");
  snek_object_t *teej = new_snek_string(vm, "Teej");
  snek_array_set(devs, 0, lane);
  snek_array_set(devs, 1, teej);

  // nothing is marked
  assert_false(devs->is_marked);
  assert_false(lane->is_marked);
  assert_false(teej->is_marked);

  // After referencing and marking, the
  // array should be marked, but not the contents
  frame_reference_object(frame, devs);
  mark(vm);
  assert_true(devs->is_marked);
  assert_false(lane->is_marked);
  assert_false(teej->is_marked);

  // After tracing, the contents should be marked
  trace(vm);
  assert_true(devs->is_marked);
  assert_true(lane->is_marked);
  assert_true(teej->is_marked);

  vm_free(vm);
  assert(boot_all_freed());
});

munit_case(SUBMIT, test_trace_nested, {
  vm_t *vm = vm_new();
  frame_t *frame = vm_new_frame(vm);

  snek_object_t *bootdevs = new_snek_array(vm, 2);
  snek_object_t *lane = new_snek_string(vm, "Lane");
  snek_object_t *hunter = new_snek_string(vm, "Hunter");
  snek_array_set(bootdevs, 0, lane);
  snek_array_set(bootdevs, 1, hunter);

  snek_object_t *terminaldevs = new_snek_array(vm, 4);
  snek_object_t *prime = new_snek_string(vm, "Prime");
  snek_object_t *teej = new_snek_string(vm, "Teej");
  snek_object_t *dax = new_snek_string(vm, "Dax");
  snek_object_t *adam = new_snek_string(vm, "Adam");
  snek_array_set(terminaldevs, 0, prime);
  snek_array_set(terminaldevs, 1, teej);
  snek_array_set(terminaldevs, 2, dax);
  snek_array_set(terminaldevs, 3, adam);

  snek_object_t *alldevs = new_snek_array(vm, 2);
  snek_array_set(alldevs, 0, bootdevs);
  snek_array_set(alldevs, 1, terminaldevs);

  frame_reference_object(frame, alldevs);
  mark(vm);
  trace(vm);

  assert_true(bootdevs->is_marked);
  assert_true(lane->is_marked);
  assert_true(hunter->is_marked);
  assert_true(terminaldevs->is_marked);
  assert_true(prime->is_marked);
  assert_true(teej->is_marked);
  assert_true(dax->is_marked);
  assert_true(adam->is_marked);
  assert_true(alldevs->is_marked);

  vm_free(vm);
  assert(boot_all_freed());
});

int main() {
  MunitTest tests[] = {
    munit_test("/test_trace_vector", test_trace_vector),
    munit_test("/test_trace_array", test_trace_array),
    munit_test("/test_trace_nested", test_trace_nested),
    munit_null_test,
  };

  MunitSuite suite = munit_suite("mark-and-sweep", tests);

  return munit_suite_main(&suite, NULL, 0, NULL);
}
