#ifndef UI_LIB_H
#define UI_LIB_H

#include "globals.h"
#include "common.h"
#include <raylib.h>
#include <stdint.h>

typedef enum
{
    UI_TYPE_PANEL,
    UI_TYPE_PANEL_9_PATCH,
    UI_TYPE_BUTTON,
    UI_TYPE_LABEL,
    UI_TYPE_IMAGE,
    UI_TYPE_CANVAS
} ui_type_t;

#define UI_ENTITY_ID_INVALID -1

typedef struct ui_id_t
{
    int16_t layer;
    int16_t idx;
} ui_id_t;

typedef struct ui_t
{
    dynarr_t data; // layered structure
} ui_t;

typedef struct ui_entity_t
{
    ivec_t screen_pos; // position of left upper corner of rectangle in screen coordinate system
    ivec_t base_pos; // position to base point (size * base_alignment) from parent point with (position = parent->size * parent_alignment) in parent coordinate system
    ivec_t size; // vector of size thjat goes from screen_pos to right bottom corner
    Vector2 base_align; // 0 to 1
    Vector2 parent_align; // 0 to 1
    int is_visible;
    
    ui_type_t type;

    int16_t parent;
    int16_t first_child;
    int16_t next_sibling;
    int16_t prev_sibling;
    int16_t layer;
    int16_t self_id;

    union
    {
        struct // UI_TYPE_PANEL
        {
            int has_borders;
            Color background_color;
            Color border_color;
        } panel;

        struct // UI_TYPE_PANEL
        {
            //NPatchInfo patch;
            int left;         
            int top;          
            int right;        
            int bottom;       

            texture_id_t tex_id;
            Color tint;
        } panel_9_patch;

        struct // UI_TYPE_LABEL
        {
            const char* text;
            Color text_color;
        } label;

        struct
        {
            void * custom_ptr1;
            void * custom_ptr2;
            void (*on_click)(struct ui_entity_t*);
            void (*on_hover)(struct ui_entity_t*);
            void (*on_enter)(struct ui_entity_t*);
            void (*on_exit)(struct ui_entity_t*);
            int is_selected;
        } button;

        struct 
        {
            texture_id_t tex_id;
        } image;
    };
} ui_entity_t;

typedef void(*button_cb)(struct ui_entity_t*);  

ui_t ui_allocate(int precreated_layers, int preallocation_per_layer)
{
    ui_t container;
    container.data = dynarr_allocate(sizeof(dynarr_t),precreated_layers, precreated_layers * 2);
    dynarr_t* layers = container.data.data;

    for (int i = 0; i < container.data.size; i++)
        layers[i] = dynarr_allocate(sizeof(ui_entity_t), 0, preallocation_per_layer);

    return container;
}

void ui_free(ui_t* container)
{
    dynarr_t* layers = container->data.data;

    for (int i = 0; i < container->data.size; i++)
        dynarr_free(layers + i);

    dynarr_free(&container->data);
}

ui_entity_t* ui_get_entity(ui_t* ui, const ui_id_t* id)
{
    dynarr_t* layer = dynarr_get(&ui->data, id->layer);
    return dynarr_get(layer, id->idx);
}

ui_id_t ui_add_child(ui_t* container, const ui_id_t* entity, ui_entity_t* child)
{
    int16_t layer_id = entity ? entity->layer + 1 : 0;
    int16_t parent_id = entity ? entity->idx : UI_ENTITY_ID_INVALID;
    int16_t next_sibling_id = UI_ENTITY_ID_INVALID;
    int16_t first_child_id = UI_ENTITY_ID_INVALID;

    int16_t prev_sibling_id;
    if (layer_id >= container->data.size)
    {
        dynarr_t new_layer = dynarr_allocate(sizeof(ui_entity_t), 0, 32);
        dynarr_add(&container->data, &new_layer);
    }
    dynarr_t* layer = dynarr_get(&container->data, layer_id);
    int16_t self_id = layer->size;

    // find prev sibling
    if (entity)
    {
        ui_entity_t* e = ui_get_entity(container, entity);

        prev_sibling_id = e->first_child;
        if (prev_sibling_id != UI_ENTITY_ID_INVALID)
        {
            ui_entity_t* sibling = dynarr_get(layer, prev_sibling_id);
            while(sibling->next_sibling != UI_ENTITY_ID_INVALID)
            {
                sibling = dynarr_get(layer, sibling->next_sibling);
                prev_sibling_id = sibling->self_id;
            };
        }
    }
    else
    {
        prev_sibling_id = self_id ? self_id - 1 : UI_ENTITY_ID_INVALID;
    }

    // set parent data
    if (entity)
    {
        ui_entity_t* e = ui_get_entity(container, entity);
        if (e->first_child == UI_ENTITY_ID_INVALID)
            e->first_child = self_id;
    }

    // set prev sibling data
    if (prev_sibling_id != UI_ENTITY_ID_INVALID)
    {
        ui_entity_t* sibling = dynarr_get(layer, prev_sibling_id);
        sibling->next_sibling = self_id;
    }

    // set main data
    child->layer = layer_id;
    child->self_id = self_id;
    child->parent = parent_id;
    child->next_sibling = next_sibling_id;
    child->prev_sibling = prev_sibling_id;
    child->first_child = first_child_id;
 
    dynarr_add(layer, child);
    return (ui_id_t)
    {
        .idx = child->self_id,
        .layer = child->layer
    };
}

void ui_process(ui_t* container, int screen_w, int screen_h)
{
    Vector2 mouse_pos_v = GetMousePosition();
    ivec_t mouse_pos = {mouse_pos_v.x, mouse_pos_v.y};
    dynarr_t* layers = container->data.data;

    // positioning of root layer
    if (layers->size > 0)
    {
        ivec_t screen_size = {screen_w, screen_h};
        ui_entity_t* layer = layers->data;
        for (int entity_id = 0; entity_id < layers->size; entity_id++)
        {
            ui_entity_t* e = layer + entity_id;
            ivec_t parent_pivot = ivec_mul_v2(screen_size, e->parent_align);
            ivec_t base_local = ivec_mul_v2(e->size, e->base_align);
            e->screen_pos = ivec_add(parent_pivot, ivec_sub(e->base_pos, base_local));
        }
    }

    // positioning rest of layers
    for (int i = 1; i < container->data.size; i++)
    {
        dynarr_t* layer_arr = layers + i;
        dynarr_t* layer_arr_parent = layers + i - 1;
        ui_entity_t* layer = layer_arr->data;
        ui_entity_t* layer_parent = layer_arr_parent->data;
        for (int entity_id = 0; entity_id < layer_arr->size; entity_id++)
        {
            ui_entity_t* e = &layer[entity_id];
            ui_entity_t* parent = &layer_parent[e->parent];

            ivec_t parent_pivot = ivec_mul_v2(parent->size, e->parent_align);
            ivec_t base_local = ivec_mul_v2(e->size, e->base_align);
            e->screen_pos = ivec_add(parent->screen_pos, 
                    ivec_add(parent_pivot, ivec_sub(e->base_pos, base_local)));
            
            e->is_visible = parent->is_visible;
        }
    }

    // process button callbacks
    for (int i = 0; i < container->data.size; i++)
    {
        dynarr_t* layer_arr = layers + i;
        ui_entity_t* layer = layer_arr->data;
        for (int entity_id = 0; entity_id < layer_arr->size; entity_id++)
        {
            ui_entity_t* e = &layer[entity_id];
            
            if (e->type != UI_TYPE_BUTTON) continue;
            
            ivec_t cursor_local = ivec_sub(mouse_pos, e->screen_pos);
            int is_outside = cursor_local.x < 0 || cursor_local.y < 0 
                    || cursor_local.x > e->size.x || cursor_local.y > e->size.y;
        
            if (is_outside && e->button.is_selected)
            {
                if (e->button.on_exit) 
                    e->button.on_exit(e);
                e->button.is_selected = 0;
            }

            if (is_outside)
                continue;

            if (!is_outside && e->button.is_selected == 0)
            {
                if (e->button.on_enter)
                    e->button.on_enter(e);
                e->button.is_selected = 1;
            }
            if (e->button.on_hover)
                e->button.on_hover(e);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && e->button.on_click)
                e->button.on_click(e);
        }
    }
}

#endif // UI_LIB_H
