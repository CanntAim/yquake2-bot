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
#include "../sound/header/local.h"

player_state_t TestPlayerState(){
  player_state_t player_state;

  player_state.viewangles[0] = 0.0;
  player_state.viewangles[1] = 0.0;
  player_state.viewangles[2] = 0.0;

  player_state.viewoffset[0] = 0.0;
  player_state.viewoffset[1] = 0.0;
  player_state.viewoffset[2] = 0.0;

  player_state.kick_angles[0] = 0.0;
  player_state.kick_angles[1] = 0.0;
  player_state.kick_angles[2] = 0.0;

  player_state.gunangles[0] = 0.0;
  player_state.gunangles[1] = 0.0;
  player_state.gunangles[2] = 0.0;

  player_state.gunoffset[0] = 0.0;
  player_state.gunoffset[1] = 0.0;
  player_state.gunoffset[2] = 0.0;

  player_state.gunindex = 0;
  player_state.gunframe = 0;

  player_state.blend[0] = 0;
  player_state.blend[1] = 0;
  player_state.blend[2] = 0;
  player_state.blend[3] = 0;

  player_state.fov = 0;
  player_state.rdflags = 0;

  player_state.stats[STAT_HEALTH] = 100;
  player_state.stats[STAT_ARMOR] = 100;
  player_state.stats[STAT_TIMER] = 100;
  player_state.stats[STAT_FRAGS] = 100;
  player_state.stats[STAT_ARMOR_ICON] = 5;
  player_state.stats[STAT_AMMO_ICON] = 5;

  return player_state;
}

entity_t TestEntity(){
  entity_t entity;
  char *test = "test";
  R_RegisterModel(test);

  entity.angles[0] = 0.0;
  entity.angles[1] = 0.0;
  entity.angles[2] = 0.0;

  entity.origin[0] = 0.0;
  entity.origin[1] = 0.0;
  entity.origin[2] = 0.0;

  entity.frame = 1;
  entity.oldframe = 1;

  entity.oldorigin[0] = 0.0;
  entity.oldorigin[1] = 0.0;
  entity.oldorigin[2] = 0.0;

  entity.backlerp = 0.0;
  entity.skinnum = 0;
  entity.lightstyle = 0;
  entity.alpha = 0.0;
  entity.flags = 0;

  return entity;
}

refdef_t TestRefDef(){ 
  refdef_t refdef;
  byte areabits;

  refdef.x = 0;
  refdef.y = 0;
  refdef.width = 10;
  refdef.height = 10;
  refdef.fov_x = 90.0;
  refdef.fov_y = 90.0;

  refdef.vieworg[0] = 0.0;
  refdef.vieworg[1] = 0.0;
  refdef.vieworg[2] = 0.0;

  refdef.vieworg[0] = 0.0;
  refdef.vieworg[1] = 0.0;
  refdef.vieworg[2] = 0.0;

  refdef.blend[0] = 0.0;
  refdef.blend[1] = 0.0;
  refdef.blend[2] = 0.0;
  refdef.blend[3] = 0.0;

  refdef.time = 0.0;
  refdef.rdflags= 0;

  refdef.num_entities = 4;
  refdef.num_dlights = 10;
  refdef.num_particles = 10;
  refdef.areabits = &areabits;
  memcpy(refdef.areabits, "1", 1);

  entity_t entities[refdef.num_entities];
  entities[0] = TestEntity();
  entities[1] = TestEntity();
  entities[2] = TestEntity(); 
  entities[3] = TestEntity();
  refdef.entities = entities;

  return refdef;
}

channel_t TestChannel(){ 
  channel_t channel;
  sfxcache_t cache;
  sfx_t sfx;

  channel.sfx = &sfx;
  channel.sfx->registration_sequence = 5;
  channel.sfx->cache = &cache;
  channel.sfx->cache->length = 10;
  channel.sfx->cache->loopstart = 10;
  channel.sfx->cache->speed = 10;
  channel.sfx->cache->width = 10;
  channel.sfx->cache->size = 10;
  channel.sfx->cache->bufnum = 10;
  channel.sfx->cache->stereo = 1;
  channel.sfx->truename = "test";

  memcpy(channel.sfx->cache->data, "1", 1); 
  memcpy(channel.sfx->name, "test", 5);

  channel.leftvol = 255;
  channel.rightvol = 255;
  channel.end = 0;
  channel.pos = 0;
  channel.looping = -1;
  channel.entnum = 1;
  channel.entchannel = 1;
  channel.origin[0] = 0;
  channel.origin[1] = 0;
  channel.origin[2] = 0;
  channel.dist_mult = 0;
  channel.master_vol = 255;
  channel.fixed_origin = true;
  channel.autosound = true;
  return channel;
}

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
  printf("BLAH!");
  return (ssize_t)mock_type(int);
}

char *__wrap_GymModel(entity_t *entity);
char *__wrap_GymModel(entity_t *entity)
{
  printf("BLAH!");
  return mock_type(char);
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

static void TestGymCaptureCurrentPlayerViewStateCL(void **state) {
  refdef_t refdef = TestRefDef();
  player_state_t playerstate = TestPlayerState();

  char str[4];
  will_return(__wrap_GymModel, str);
 
  GymCaptureCurrentPlayerViewStateCL(refdef, playerstate);
}

static void TestGymCaptureCurrentPlayerSoundStateCL(void **state) {
  channel_t channel = TestChannel();
 
  char str[10000];
  int expected = sizeof(str);
  will_return(__wrap_write, expected);

  GymCaptureCurrentPlayerSoundStateCL(&channel);
}

static void TestGymMessageToBuffer(void **state) {
  message_t message = TestMessage();

  message.playerPositionX = 2;
  message.playerPositionY = 2;
  GymReadySet(true);

  char str[10000];
  int expected = sizeof(str);
  will_return(__wrap_write, expected);

  GymMessageToBuffer(message, str);
  //assert_float_equal(message.playerPositionX, 2.0, 1.0);
  //assert_float_equal(message.playerPositionX, 2.0, 1.0);
}

int RunTests(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(TestTrim),
    cmocka_unit_test(TestTrimLeft),
    cmocka_unit_test(TestTrimRight),
    cmocka_unit_test(TestGymMessageToBuffer),
    cmocka_unit_test(TestGymCaptureCurrentPlayerSoundStateCL),
    cmocka_unit_test(TestGymCaptureCurrentPlayerViewStateCL)
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
