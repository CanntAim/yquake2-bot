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

#include "header/client.h"

void
GymCapturePlayerStateCL(refdef_t refdef, player_state_t state){
	/* Seperate frame */
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
  //printf("Inventory: %d\n", state.stat[STAT_HEALTH]);
  printf("Current Armor Type: %d\n", state.stats[STAT_ARMOR_ICON]);
  printf("Current Weapon: %d\n", state.stats[STAT_AMMO_ICON]);

	printf("...Entities for current frame:\n");
	printf("...\n");
}

void
GymCaptureEntityStateCL(refdef_t refdef, entity_t *entity){
	printf("drawing entity: %s ...x: %f ...y: %f ...z: %f\n", entity->model,
	entity->origin[0],
	entity->origin[1],
	entity->origin[2]);
  qboolean front = GymCheckIfInFrontCL(refdef.viewangles,
    refdef.vieworg,
    entity->origin);
  qboolean looking = GymCheckIfInFrontCL(entity->angles,
    entity->origin,
    refdef.vieworg);
  qboolean visible = GymCheckIfIsVisbleCL(refdef.viewangles,
    refdef.vieworg,
    entity->origin);
  printf("Is in front of player: %d\n", front);
  printf("Is visible to player: %d\n", visible);
  printf("Entity looking at player: %d\n", looking);
}

qboolean GymCheckIfIsVisbleCL(float view[3], float source[3], float dest[3]){
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
  trace_t trace;

  trace = CM_BoxTrace(source_vec, dest_vec, vec3_origin, vec3_origin, 0, MASK_OPAQUE);
  if (trace.fraction == 1.0)
  {
    return true;
  }

  return false;
}

qboolean
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
    return true;
  }

  return false;
}

void
GymCaptureCurrentPlayerViewStateCL(refdef_t refdef, player_state_t state)
{
  entity_t *entity;
  GymCapturePlayerStateCL(refdef, state);
  for (int i = 0; i < refdef.num_entities; i++)
	{
    entity = &refdef.entities[i];
		GymCaptureEntityStateCL(refdef, entity);
  }
}

void GymCaptureCurrentPlayerSoundStateCL(channel_t *ch)
{
  printf("Player heard sound: %s ...x: %f ...y: %f ...z: %f\n",
	ch->sfx->name,
	ch->origin[0],
  ch->origin[1],
  ch->origin[2]);
}
