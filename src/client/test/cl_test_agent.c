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
#include "../header/test.h"
#include "../header/client.h"


message_t TestMessage(){
  message_t message;
  message.playerPositionX = -99999.0;
  message.playerPositionY = -99999.0;
  message.playerPositionZ = -99999.0;
  message.playerViewAngleX = -99999.0;
  message.playerViewAngleY = -99999.0;
  message.playerViewAngleZ = -99999.0;
  message.time = -1;
  message.frags = -1;
  message.enemyLooking = -1;
  message.enemyPositionX = -99999.0;
  message.enemyPositionY = -99999.0;
  message.enemyPositionZ = -99999.0;
  message.projectileDistance = -99999.0;
  return message;
}

/* Test cases for the RL agent methods */
ssize_t __wrap_write(int fd, const void *buf, size_t count);
ssize_t __wrap_write(int fd, const void *buf, size_t count)
{
  return (ssize_t)mock_type(int);
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
  char str[10000];
  message_t message = TestMessage();
  int expected = sizeof(str);
  message.playerPositionX = 2;
  message.playerPositionY = 2;
 
  GymReadySet(true);
  will_return(__wrap_write, expected);
  
  GymMessageToBuffer(message, str);
  //assert_float_equal(message.playerPositionX, 2.0, 1.0);
  //assert_float_equal(message.playerPositionX, 2.0, 1.0);
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
