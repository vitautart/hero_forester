#ifndef GLOBALS_H
#define GLOBALS_H

#include "common.h"

#define TSIZE               32

#define MASK_BIT_DIR_LEFT   0b0001
#define MASK_BIT_DIR_UP     0b0010
#define MASK_BIT_DIR_RIGHT  0b0100
#define MASK_BIT_DIR_DOWN   0b1000

typedef enum scene_type_t
{
    SCENE_TYPE_MAIN_MENU,
    SCENE_TYPE_LOCATION
} scene_type_t;

typedef struct app_state_t
{
    scene_type_t current_scene;
} app_state_t;

typedef enum
{
    MASK_DIR_LU = MASK_BIT_DIR_LEFT | MASK_BIT_DIR_UP,
    MASK_DIR_RU = MASK_BIT_DIR_RIGHT | MASK_BIT_DIR_UP,
    MASK_DIR_RD = MASK_BIT_DIR_RIGHT | MASK_BIT_DIR_DOWN,
    MASK_DIR_LD = MASK_BIT_DIR_LEFT | MASK_BIT_DIR_DOWN,
    MASK_DIR_TSHAPE_L = MASK_BIT_DIR_LEFT | MASK_BIT_DIR_DOWN | MASK_BIT_DIR_UP,
    MASK_DIR_TSHAPE_U = MASK_BIT_DIR_UP | MASK_BIT_DIR_LEFT | MASK_BIT_DIR_RIGHT,
    MASK_DIR_TSHAPE_R = MASK_BIT_DIR_RIGHT | MASK_BIT_DIR_DOWN | MASK_BIT_DIR_UP,
    MASK_DIR_TSHAPE_D = MASK_BIT_DIR_DOWN | MASK_BIT_DIR_LEFT | MASK_BIT_DIR_RIGHT,
    MASK_DIR_CROSS = MASK_BIT_DIR_DOWN | MASK_BIT_DIR_LEFT | MASK_BIT_DIR_RIGHT | MASK_BIT_DIR_UP
} mask_dir_t;

typedef enum
{
    TEXTURE_ID_PLAYER_1,
    TEXTURE_ID_PLAYER_2,
    TEXTURE_ID_PLAYER_3,
    TEXTURE_ID_PLAYER_4,
    TEXTURE_ID_PLAYER_5,
    TEXTURE_ID_PLAYER_6,
    TEXTURE_ID_PLAYER_7,
    TEXTURE_ID_PLAYER_8,
    TEXTURE_ID_TREE_1,
    TEXTURE_ID_TREE_2,
    TEXTURE_ID_TREE_3,
    TEXTURE_ID_TREE_4,
    TEXTURE_ID_TREE_5,
    TEXTURE_ID_TREE_6,
    TEXTURE_ID_TREE_7,
    TEXTURE_ID_TREE_8,
    TEXTURE_ID_ENEMY_1,
    TEXTURE_ID_ENEMY_2,
    TEXTURE_ID_ENEMY_3,
    TEXTURE_ID_ENEMY_4,
    TEXTURE_ID_ENEMY_5,
    TEXTURE_ID_ENEMY_6,
    TEXTURE_ID_ENEMY_7,
    TEXTURE_ID_ENEMY_8,
    TEXTURE_ID_GRASS_1,
    TEXTURE_ID_GRASS_2,
    TEXTURE_ID_GRASS_3,
    TEXTURE_ID_GRASS_4,
    TEXTURE_ID_GRASS_5,
    TEXTURE_ID_GRASS_6,
    TEXTURE_ID_GRASS_7,
    TEXTURE_ID_GRASS_8,
    TEXTURE_ID_PATHLINE_HORIZONTAL,
    TEXTURE_ID_PATHLINE_VERICAL,
    TEXTURE_ID_PATHLINE_3,
    TEXTURE_ID_PATHLINE_4,
    TEXTURE_ID_PATHTURN_LU,
    TEXTURE_ID_PATHTURN_RU,
    TEXTURE_ID_PATHTURN_RD,
    TEXTURE_ID_PATHTURN_LD,
    TEXTURE_ID_PATHTURN_TSHAPE_L,
    TEXTURE_ID_PATHTURN_TSHAPE_U,
    TEXTURE_ID_PATHTURN_TSHAPE_R,
    TEXTURE_ID_PATHTURN_TSHAPE_D,
    TEXTURE_ID_PATHTURN_CROSS,
    TEXTURE_ID_EFFECT_1,
    TEXTURE_ID_EFFECT_2,
    TEXTURE_ID_EFFECT_3,
    TEXTURE_ID_EFFECT_4,
    TEXTURE_ID_EFFECT_5,
    TEXTURE_ID_EFFECT_6,
    TEXTURE_ID_EFFECT_7,
    TEXTURE_ID_EFFECT_8,
    TEXTURE_ID_BUTTON_1,
    TEXTURE_ID_MAX_VALUE,
    TEXTURE_ID_INVALID
} texture_id_t;

static texture_id_t global_tex_id_right_to_left_mapping[TEXTURE_ID_MAX_VALUE];
static texture_id_t global_tex_id_left_to_right_mapping[TEXTURE_ID_MAX_VALUE];

//#define DEBUG_RENDER
// GLOBAL CONTAINERS
static minheap_t global_open_set;
static hashmap_t global_path_links;
static hashmap_t global_g_score;
static dynarr_t global_path;
static dynarr_t global_bresenham_line;
#ifdef DEBUG_RENDER
static dynarr_t global_debug_red_map_cell_pos;
static dynarr_t global_debug_blue_map_cell_pos;
static dynarr_t global_debug_grey_map_cell_pos;
static dynarr_t global_debug_yellow_map_cell_pos;
#endif

void globals_allocate_per_session()
{
#ifdef DEBUG_RENDER
    global_debug_red_map_cell_pos = dynarr_allocate(sizeof(ivec_t), 0, 128);
    global_debug_blue_map_cell_pos = dynarr_allocate(sizeof(ivec_t), 0, 128);
    global_debug_grey_map_cell_pos = dynarr_allocate(sizeof(ivec_t), 0, 128);
    global_debug_yellow_map_cell_pos = dynarr_allocate(sizeof(ivec_t), 0, 128);
#endif

}

void globals_allocate_per_location()
{
    global_open_set = minheap_allocate(10000);
    global_path_links = hashmap_allocate(128, 64);
    global_g_score = hashmap_allocate(128, 64);
    global_path = dynarr_allocate(sizeof(qnode_t), 0, 128);
    global_bresenham_line = dynarr_allocate(sizeof(ivec_t), 0, 128);
}

void globals_free_per_location()
{
    minheap_free(&global_open_set);
    hashmap_free(&global_path_links);
    hashmap_free(&global_g_score);
    dynarr_free(&global_path);
    dynarr_free(&global_bresenham_line);
}

void globals_free_per_session()
{
#ifdef DEBUG_RENDER
    dynarr_free(&global_debug_red_map_cell_pos);
    dynarr_free(&global_debug_blue_map_cell_pos);
    dynarr_free(&global_debug_grey_map_cell_pos);
    dynarr_free(&global_debug_yellow_map_cell_pos);
#endif
}

#endif // GLOBALS_H
