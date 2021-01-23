#include "globals.h"
#include "common.h"
#include <raylib.h>
#include <stdint.h>

typedef enum
{
    UI_TYPE_PANEL,
    UI_TYPE_BUTTON,
    UI_TYPE_LABEL,
    UI_TYPE_IMAGE,
    UI_TYPE_CANVAS
} ui_type_t;

#define UI_ENTITY_ID_INVALID -1

typedef struct ui_entity_t
{
    ivec_t pos;
    ivec_t size;
    Vector2 rel_pos; // from 0 to 1
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
            Color backgroud_color;
            Color border_color;
        } panel;

        struct // UI_TYPE_LABEL
        {
            const char* text;
        } label;

        struct
        {
            void * custom_ptr1;
            void * custom_ptr2;
            void (*on_click)(struct ui_entity_t*);
            void (*on_exit)(struct ui_entity_t*);
            void (*on_enter)(struct ui_entity_t*);
        } button;
        struct 
        {
            texture_id_t id;
        } image;
    };
} ui_entity_t;


// structure 
typedef struct ui_container_t
{
    dynarr_t data; // layered structure
} ui_container_t;

ui_container_t ui_allocate(int precreated_layers, int preallocation_per_layer)
{
    ui_container_t container;
    container.data = dynarr_allocate(sizeof(dynarr_t),precreated_layers, precreated_layers * 2);
    dynarr_t* layers = container.data.data;

    for (int i = 0; i < container.data.size; i++)
        layers[i] = dynarr_allocate(sizeof(ui_entity_t), 0, preallocation_per_layer);

    return container;
}

void ui_free(ui_container_t* container)
{
    dynarr_t* layers = container->data.data;

    for (int i = 0; i < container->data.size; i++)
        dynarr_free(layers + i);

    dynarr_free(&container->data);
}

void ui_add_child(ui_container_t* container, ui_entity_t* entity, ui_entity_t* child)
{
    int entity_layer_id = entity != NULL ? entity->layer : -1;

    if (entity_layer_id >=container->data.size)
    {
        dynarr_t new_layer = dynarr_allocate(sizeof(dynarr_t), 0, 32);
        dynarr_add(&container->data, &new_layer);
    }

    dynarr_t* layer = dynarr_get(&container->data, entity_layer_id + 1);

    child->first_child = UI_ENTITY_ID_INVALID;
    child->parent = entity != NULL ? entity->self_id : UI_ENTITY_ID_INVALID;
    child->self_id = layer->size;
    child->next_sibling = UI_ENTITY_ID_INVALID;

    if (entity == NULL)
    {
        child->prev_sibling = UI_ENTITY_ID_INVALID;
    }
    else if (entity->first_child != UI_ENTITY_ID_INVALID)
    {
        ui_entity_t* sibling = dynarr_get(layer, entity->first_child);
        while(sibling->next_sibling != UI_ENTITY_ID_INVALID)
        {
            sibling = dynarr_get(layer, sibling->next_sibling);
        };
        sibling->next_sibling = child->self_id;
        child->prev_sibling = sibling->self_id;
    }
    else
    {
        child->prev_sibling = UI_ENTITY_ID_INVALID;
        entity->first_child = child->self_id;
    }
    dynarr_add(layer, child);
}

void ui_remove_entity(ui_container_t* container, ui_entity_t* entity)
{
    if (entity->first_child != UI_ENTITY_ID_INVALID)
    {
        dynarr_t* child_layer = dynarr_get(&container->data, entity->layer + 1);
        ui_entity_t* child = dynarr_get(child_layer, entity->first_child);
        int16_t child_next_id = child->next_sibling;
        ui_remove_entity(container, child);
        while(child_next_id != UI_ENTITY_ID_INVALID)
        {
            child = dynarr_get(child_layer, child_next_id);
            ui_remove_entity(container, child);
        }
    }

    int16_t entity_id = entity->self_id;
    int16_t entity_layer = entity->layer;
    int16_t entity_first_child = entity->first_child;
    dynarr_t* layer = dynarr_get(&container->data, entity_layer);

    if (entity->parent != UI_ENTITY_ID_INVALID)
    {
        ui_entity_t* parent = dynarr_get(layer, entity->parent);
        if (parent->first_child == entity_id)
            parent->first_child = entity->next_sibling;
    }
    if (entity->next_sibling != UI_ENTITY_ID_INVALID)
    {
        ui_entity_t* sibling = dynarr_get(layer, entity->next_sibling);
        sibling->prev_sibling = entity->prev_sibling;
    }
    if (entity->prev_sibling != UI_ENTITY_ID_INVALID)
    {
        ui_entity_t* sibling = dynarr_get(layer, entity->prev_sibling);
        sibling->next_sibling = entity->next_sibling;
    }

    if (dynarr_remove_swap(layer, entity_id))
    {
        ui_entity_t* swaped_entity = dynarr_get(layer, entity_id);
        if (swaped_entity->parent != UI_ENTITY_ID_INVALID)
        {
            ui_entity_t* parent = dynarr_get(layer, swaped_entity->parent);
            if (parent->first_child == swaped_entity->self_id)
                parent->first_child = entity_id;
        }
        swaped_entity->self_id = entity_id;
        if (swaped_entity->next_sibling != UI_ENTITY_ID_INVALID)
        {
            ui_entity_t* sibling = dynarr_get(layer, swaped_entity->next_sibling);
            sibling->prev_sibling = entity_id;
        }
        if (swaped_entity->prev_sibling != UI_ENTITY_ID_INVALID)
        {
            ui_entity_t* sibling = dynarr_get(layer, swaped_entity->prev_sibling);
            sibling->next_sibling = entity_id;
        }
        if (swaped_entity->first_child != UI_ENTITY_ID_INVALID)
        {
            dynarr_t* child_layer = dynarr_get(&container->data, entity_layer + 1);
            ui_entity_t* child = dynarr_get(child_layer, swaped_entity->first_child);
            child->parent = entity_id;
            while(child->next_sibling != UI_ENTITY_ID_INVALID)
            {
                child = dynarr_get(child_layer, child->next_sibling);
                child->parent = entity_id;
            };
        }
    }
}

void ui_process(ui_container_t* ui_data)
{
    ui_entity_t* container = ui_data->data.data;
}
