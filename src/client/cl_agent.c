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
 * This file implements the agent state gathering subroutine. To help train
 * an RL bot.
 * =======================================================================
 */


#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "header/client.h"

qboolean Learn = false;
qboolean Render = false;
qboolean Ready = false;
qboolean OpenSocket = false; 
qboolean SilentSoundCapture = true;
qboolean SilentPlayerStateCapture = true;
qboolean SilentEntityCapture = true;
message_t Message;
char Map[1024];

int connfd, conncl, connrc;

void error(char *msg)
{
  perror(msg);
  exit(1);
}

void
GymInitializeMessage(){
  Message.playerPositionX = -99999.0;
  Message.playerPositionY = -99999.0;
  Message.playerPositionZ = -99999.0;
  Message.playerViewAngleX = -99999.0;
  Message.playerViewAngleY = -99999.0;
  Message.playerViewAngleZ = -99999.0;
  Message.time = -1;
  Message.frags = -1;
  Message.enemyLooking = -1;
  Message.enemyPositionX = -99999.0;
  Message.enemyPositionY = -99999.0;
  Message.enemyPositionZ = -99999.0;
  Message.projectileDistance = -99999.0;
}

void
GymStartGameServerAndSetRules(char startmap[1024], float timelimit, float fraglimit,
	    float maxclients, char hostname[1024])
{
  strncpy(Map, startmap, 1024);  
  Cvar_SetValue("maxclients", maxclients);
  Cvar_SetValue("timelimit", timelimit);
  Cvar_SetValue("fraglimit", fraglimit);
  Cvar_Set("hostname", hostname);

  Cvar_SetValue("deathmatch", 1); /* deathmatch is always true for rogue games */

  if (Com_ServerState())
  {
    Cbuf_AddText("disconnect\n");
  }

  Cbuf_AddText("cl_showfps 0\n");
  Cbuf_AddText(va("map %s\n", startmap));
}

void
GymJoinGameServer(char address[20]) {
  char buffer[128];
  Com_sprintf(buffer, sizeof(buffer), "connect %s\n", address);
  Cbuf_AddText(buffer);
}

void
GymStartServer(){
  if(!OpenSocket){
    pthread_t thread;
    pthread_create(&thread, NULL, GymOpenSocket, NULL);
    GymInitializeMessage();
    OpenSocket = true;
  }
}

qboolean
GymRender(){
  return Render;
}

void*
GymOpenSocket(void *args){
  struct sockaddr_un addr;
  char *socket_path = "../quake_socket";
  char buf[500];
  
  if ((connfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
  unlink(socket_path);

  if (bind(connfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind error");
    exit(-1);
  }

  if (listen(connfd, 5) == -1) {
    perror("listen error");
    exit(-1);
  }

  while (1) {
    if ((conncl = accept(connfd, NULL, NULL)) == -1) {
      perror("accept error");
      continue;
    }

    while ((connrc = read(conncl, buf, sizeof(buf))) > 0) {
      printf("read %u bytes: %.*s\n", connrc, connrc, buf);
      if(!Ready){
	char *purpose = strtok(buf, ",");
	char *map = strtok(NULL, ",");
	char *address = strtok(NULL, ",");
	char *render = strtok(NULL, ",");

	if (strcmp("y", render) == 0) {
	  Render = true;
	}

	if (strcmp("start server", purpose) == 0) {
	  GymStartGameServerAndSetRules(map, 10.0, 0.0, 2.0, address);
	  snprintf(buf, 500, "successfully started server");
	  write(conncl, buf, strlen(buf));
	  Ready = true;
	} else if (strcmp("connect to server", purpose) == 0) {
	  GymJoinGameServer(address);
	  snprintf(buf, 500, "successfully joined server");
	  write(conncl, buf, strlen(buf));
	  Ready = true;
	  Learn = true;
	}
      } else {
	char *command = strtok(buf, ".");
	
	if(strcmp("attackup", command) == 0) {
	  IN_AttackUp();
	} else if (strcmp("forwardup", command) == 0) {
	  IN_ForwardUp();
	} else if (strcmp("forwarddown", command) == 0) {
	  IN_ForwardDown();
        } else if(strcmp("attackdown", command) == 0) {
	  IN_AttackDown();
	} else if (strcmp("attackup", command) == 0) {
	  IN_AttackUp();
        } else if (strcmp("backup", command) == 0) {
	  IN_BackUp();
	} else if (strcmp("backdown", command) == 0) {
	  IN_BackDown();
	}
      }
    }

    if (connrc == -1) {
      perror("read");
      exit(-1);
    }

    else if (connrc == 0) {
      printf("EOF\n");
      close(conncl);
    }
  }
}

void
GymCapturePlayerStateCL(refdef_t refdef, player_state_t state){
  /* Seperate frame */
  if(!SilentPlayerStateCapture){
    printf("...\n");
    printf("...Current View:\n");
    printf("...\n");
    printf("Position of player: ...x: %f ...y: %f ...z: %f\n",
	   refdef.vieworg[0],
	   refdef.vieworg[1],
	   refdef.vieworg[2]);
    printf("View angle of player: ...x: %f ...y: %f ...z: %f\n",
	   refdef.viewangles[0],
	   refdef.viewangles[1],
	   refdef.viewangles[2]);
    printf("Player Static State: \n");
    printf("Health: %d\n", state.stats[STAT_HEALTH]);
    printf("Armor: %d\n", state.stats[STAT_ARMOR]);
    printf("Time: %d\n", state.stats[STAT_TIMER]);
    printf("Frags: %d\n", state.stats[STAT_FRAGS]);
    printf("Current Armor Type: %d\n", state.stats[STAT_ARMOR_ICON]);
    printf("Current Weapon: %d\n", state.stats[STAT_AMMO_ICON]);

    printf("...Entities for current frame:\n");
    printf("...\n");
  }
  Message.playerPositionX = refdef.vieworg[0];
  Message.playerPositionY = refdef.vieworg[1];
  Message.playerPositionZ = refdef.vieworg[2];
  Message.playerViewAngleX = refdef.viewangles[0];
  Message.playerViewAngleY = refdef.viewangles[1];
  Message.playerViewAngleZ = refdef.viewangles[2];
  Message.time = state.stats[STAT_TIMER];
  Message.frags = state.stats[STAT_FRAGS];
}

void
GymCaptureEntityStateCL(refdef_t refdef, entity_t *entity, float prior){
  int front = GymCheckIfInFrontCL(refdef.viewangles,
					 refdef.vieworg,
					 entity->origin);

  int looking = GymCheckIfInFrontCL(entity->angles,
					   entity->origin,
					   refdef.vieworg);

  int visible = GymCheckIfIsVisibleCL(refdef.vieworg,
					   entity->origin);

  float distance = GymCheckDistanceTo(refdef.vieworg,
				      entity->origin);
  
  if(!SilentEntityCapture){
    printf("drawing entity: %s ...x: %f ...y: %f ...z: %f\n", (char*)entity->model,
	   entity->origin[0],
	   entity->origin[1],
	   entity->origin[2]);
    printf("Is in front of player: %d\n", front);
    printf("Is visible to player: %d\n", visible);
    printf("Entity looking at player: %d\n", looking);
  }

  const char *actual = (char*)entity->model;
  const char *player = "dmspot";
  const char *rocket = "rocket";
  const char *grenade = "grenade";
  if(strstr(actual, player) != NULL && front && visible) {
    Message.enemyLooking = looking;
    Message.enemyPositionX = entity->origin[0];
    Message.enemyPositionY = entity->origin[1];
    Message.enemyPositionZ = entity->origin[2];
  } else if((strstr(actual, rocket) != NULL
	     || strstr(actual, grenade) != NULL)
	    && distance < Message.projectileDistance) {
    Message.projectileDistance = distance;
  }    
}

int
GymCheckIfIsVisibleCL(float source[3], float dest[3]){
  //Given variables
  vec3_t source_vec;
  vec3_t dest_vec;

  source_vec[0] = source[0];
  source_vec[1] = source[1];
  source_vec[2] = source[2];

  dest_vec[0] = dest[0];
  dest_vec[1] = dest[1];
  dest_vec[2] = dest[2];

  trace_t trace;

  trace = CM_BoxTrace(source_vec, dest_vec, vec3_origin, vec3_origin, 0, MASK_OPAQUE);
  if (trace.fraction == 1.0)
    {
      return 1;
    }
  return 0;
}

int
GymCheckIfInFrontCL(float view[3], float source[3], float dest[3]){
  //Given variables
  vec3_t source_vec;
  vec3_t dest_vec;
  vec3_t view_vec;

  source_vec[0] = source[0];
  source_vec[1] = source[1];
  source_vec[2] = source[2];

  dest_vec[0] = dest[0];
  dest_vec[1] = dest[1];
  dest_vec[2] = dest[2];

  view_vec[0] = view[0];
  view_vec[1] = view[1];
  view_vec[2] = view[2];

  //Calculated Variables
  vec3_t result_vec;
  vec3_t forward_vec;
  float dot;

  AngleVectors(view_vec, forward_vec, NULL, NULL);

  VectorSubtract(dest_vec, source_vec, result_vec);
  VectorNormalize(result_vec);
  dot = DotProduct(result_vec, forward_vec);

  if (dot > 0.3)
    {
      return 1;
    }

  return 0;
}

float
GymCheckDistanceTo(float source[3], float dest[3])
{
  vec3_t difference;
  difference[0] = dest[0] - source[0];
  difference[1] = dest[1] - source[1];
  difference[2] = dest[2] - source[2];
  return powf(powf(difference[0],2) + powf(difference[1],2) + powf(difference[2],2), .5);
}

void
GymCaptureConsole(char *line)
{
  char* fixed = trim(line);
  if(strcmp("Player entered the game.", fixed) == 0 && !Learn){
    Learn = true;
    Cbuf_AddText(va("map %s\n", Map));
  }
}

void
GymDisplayFPS(float fps)
{
  printf("frame rate: %f\n", fps);
}

void
GymCaptureCurrentPlayerViewStateCL(refdef_t refdef, player_state_t state)
{
  /* Start server */
  GymStartServer();

  char buf[10000];
  entity_t *entity;
  float prior = 99999.0;

  GymCapturePlayerStateCL(refdef, state);
  for (int i = 0; i < refdef.num_entities; i++){
    entity = &refdef.entities[i];
    GymCaptureEntityStateCL(refdef, entity, prior);
  }

  GymMessageToBuffer(Message, buf);
}

void GymCaptureCurrentPlayerSoundStateCL(channel_t *ch)
{
  if(!SilentSoundCapture){
    printf("Player heard sound: %s ...x: %f ...y: %f ...z: %f\n",
	   ch->sfx->name,
	   ch->origin[0],
	   ch->origin[1],
	   ch->origin[2]);
  }

  char buf[10000];
  vec3_t source;
  vec3_t dest;
  GymStartServer();
  source[0] = Message.playerPositionX;
  source[1] = Message.playerPositionY;
  source[2] = Message.playerPositionZ;
  dest[0] = ch->origin[0];
  dest[1] = ch->origin[1];
  dest[2] = ch->origin[2];
  Message.enemySoundDistance = GymCheckDistanceTo(source, dest);
  GymMessageToBuffer(Message, buf);
}

void GymMessageToBuffer(message_t message, char buf[10000])
{
  snprintf(buf, 10000, "%f,%f,%f,%f,%f,%f,%d,%d,%d,%f,%f,%f,%f,%f,",
	  message.playerPositionX,
	  message.playerPositionY,
	  message.playerPositionZ,
	  message.playerViewAngleX,
	  message.playerViewAngleY,
	  message.playerViewAngleZ,
	  message.time,
	  message.frags,
	  message.enemyLooking,
	  message.enemySoundDistance,
	  message.enemyPositionX,
	  message.enemyPositionY,
	  message.enemyPositionZ,
	  message.projectileDistance);
  if (Ready) {
    write(conncl, buf, strlen(buf)); 
  }
}

char *ltrim(char *s)
{
  while(isspace(*s)) s++;
  return s;
}

char *rtrim(char *s)
{
  char* back = s + strlen(s);
  while(isspace(*--back));
  *(back+1) = '\0';
  return s;
}

char *trim(char *s)
{
  return rtrim(ltrim(s)); 
}

/* Test cases for the RL agent methods */

ssize_t __wrap_write(int fd, const void *buf, size_t count);
ssize_t __wrap_write(int fd, const void *buf, size_t count)
{
  printf("HELLO WORLD!");
  ssize_t byteSize;
  check_expected_ptr(buf);
  byteSize = mock_type(int);
  return byteSize;
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
  printf("HELLO WORLD!");
  char str[10000];
  message_t message;
  message.playerPositionX = 2;
  message.playerPositionY = 2;

  //will_return(__wrap_write, 42);
  //expect_string(__wrap_write, buf, "0");
  
  GymMessageToBuffer(message, str);
  assert_int_equal(message.playerPositionX, 2);
  assert_int_equal(message.playerPositionX, 2);
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
