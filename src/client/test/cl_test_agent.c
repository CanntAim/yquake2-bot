/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * This file implements the unit test cases for the agent state gathering 
 * subroutine. To help train an RL bot.
 * =======================================================================
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../header/client.h"

/* Test cases for the RL agent methods */
ssize_t __wrap_write(int fd, const void *buf, size_t count);
ssize_t __wrap_write(int fd, const void *buf, size_t count)
{
  int byteSize;
  byteSize = mock_type(int);
  //check_expected_ptr(buf);
  return (ssize_t)byteSize;
}

static void NullTestSuccess(void **state) {
  (void) state;
}

static void TestTrim(void **state) {
  char str[50] = "  test  ";
  char *strTrimmed = trim(str);
  assert_string_equal("test", strTrimmed);
}

static void TestTrimLeft(void **state) {
  char str[50] = "  test  ";
  char *strTrimmed = ltrim(str);
  assert_string_equal("test  ", strTrimmed);
}

static void TestTrimRight(void **state) {
  char str[50] = "  test  ";
  char *strTrimmed = rtrim(str);
  assert_string_equal("  test", strTrimmed);
}

static void TestGymMessageToBuffer(void **state) {
  (void) state;
  char str[10000];
  message_t message;
  message.playerPositionX = 2;
  message.playerPositionY = 2;

  //Render = 1;
  will_return_always(__wrap_write, 42);
  //expect_string(__wrap_write, buf, "0");

  GymMessageToBuffer(message, str);
  assert_float_equal(message.playerPositionX, 2.0, 1.0);
  assert_float_equal(message.playerPositionX, 2.0, 1.0);
}

int RunTests(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(NullTestSuccess),
    cmocka_unit_test(TestTrim),
    cmocka_unit_test(TestTrimLeft),
    cmocka_unit_test(TestTrimRight),
    cmocka_unit_test(TestGymMessageToBuffer)
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
