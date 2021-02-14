#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include "common.h"
#include "globals.h"
#include <uilib.h>

typedef struct ui_system_t
{
    ui_t ui;
    ui_id_t main_menu_id;
    ui_id_t bottom_panel_id;
    int is_mouse_over_map;
} ui_system_t;

// TODO: need to be finished
void ui_default_on_enter(ui_entity_t* button)
{
    ui_t* ui = button->button.custom_ptr1;
    ui_id_t panel_id = {.idx = button->parent, .layer = button->layer - 1};
    ui_entity_t* panel = ui_get_entity(ui, &panel_id);
    panel->panel_9_patch.tint = RED;
}

void ui_default_on_exit(ui_entity_t* button)
{
    ui_t* ui = button->button.custom_ptr1;
    ui_id_t panel_id = {.idx = button->parent, .layer = button->layer - 1};
    ui_entity_t* panel = ui_get_entity(ui, &panel_id);
    panel->panel_9_patch.tint = WHITE;
}

void ui_new_button_on_click(ui_entity_t* button)
{
    ui_entity_t* main_menu = button->button.custom_ptr1;
    scene_type_t* scene_type = button->button.custom_ptr2;

    main_menu->is_visible = 0;
    *scene_type = SCENE_TYPE_LOCATION;
}

ui_id_t ui_add_child_text_button(ui_t* ui, ui_id_t* entity, const char* text,
        ivec_t size, ivec_t base_pos, Vector2 base_align, Vector2 parent_align, button_cb on_click)
{
    ui_entity_t panel = 
    {
        .base_align = base_align,
        .base_pos = base_pos,
        .size = size,
        .is_visible = 1,
        .is_mouse_shadowing = 1,
        .type = UI_TYPE_PANEL_9_PATCH,
        .parent_align = parent_align,
        .panel_9_patch = 
        {
            .left = 5,
            .top = 5,
            .right = 5,
            .bottom = 5,
            .tex_id = TEXTURE_ID_BUTTON_1,
            .tint = WHITE,
        }
    };
    ui_entity_t button = 
    {
        .base_align = {0, 0},
        .base_pos = {0, 0},
        .size = size,
        .is_visible = 1,
        .is_mouse_shadowing = 1,
        .type = UI_TYPE_BUTTON,
        .parent_align = {0, 0},
        .button = 
        {
            .custom_ptr1 = ui,
            .custom_ptr2 = NULL,
            .on_click = on_click,
            .on_enter = ui_default_on_enter,
            .on_exit = ui_default_on_exit,
            .on_hover = NULL,
            .is_selected = 0
        }
    };

    int label_height = size.y - 9;
    int label_width = MeasureText(text, label_height);
    ui_entity_t label = 
    {
        .base_align = {0.5, 0.5},
        .base_pos = {0, 0},
        .size = {label_width, label_height},
        .is_visible = 1,
        .is_mouse_shadowing = 1,
        .type = UI_TYPE_LABEL,
        .parent_align = {0.5, 0.5},
        .label = 
        {
            .text = text,
            .text_color = WHITE,
        }
    };

    ui_id_t panel_id = ui_add_child(ui, entity, &panel);
    ui_add_child(ui, &panel_id, &button);
    ui_add_child(ui, &panel_id, &label);

    return panel_id;
}

ui_id_t ui_add_main_menu(ui_t* ui, ui_id_t* entity)
{
    ivec_t panel_size = {250, 320};
    ivec_t button_size = {panel_size.x - 20, 40};
    float height_inc = 1.0 / 5.0;
    ivec_t button_base_pos = IVEC_ZERO;
    ui_entity_t panel = 
    {
        .base_align = {0, 0.5},
        .base_pos = {20, 0},
        .size = panel_size,
        .is_visible = 1,
        .is_mouse_shadowing = 1,
        .type = UI_TYPE_PANEL_9_PATCH,
        .parent_align = {0, 0.5},
        .panel_9_patch = 
        {
            .left = 5,
            .top = 5,
            .right = 5,
            .bottom = 5,
            .tex_id = TEXTURE_ID_BUTTON_1,
            .tint = WHITE,
        }
    };

    ui_id_t menu = ui_add_child(ui, NULL, &panel);
    ui_add_child_text_button(ui, &menu, "New Game", button_size, IVEC_ZERO, (Vector2){0.5, 0.5},
            (Vector2){0.5, 0.5 * height_inc}, NULL);
    ui_add_child_text_button(ui, &menu, "Load", button_size, IVEC_ZERO, (Vector2){0.5, 0.5},
            (Vector2){0.5, 1.5 * height_inc}, NULL);
    ui_add_child_text_button(ui, &menu, "Options", button_size, IVEC_ZERO, (Vector2){0.5, 0.5},
            (Vector2){0.5, 2.5 * height_inc}, NULL);
    ui_add_child_text_button(ui, &menu, "Credits", button_size, IVEC_ZERO, (Vector2){0.5, 0.5},
            (Vector2){0.5, 3.5 * height_inc}, NULL);
    ui_add_child_text_button(ui, &menu, "Exit", button_size, IVEC_ZERO, (Vector2){0.5, 0.5},
            (Vector2){0.5, 4.5 * height_inc}, NULL);

    return menu;
}

// TODO: need to be finished
ui_id_t ui_add_ingame_bottom_panel(ui_t* ui, ui_id_t* entity)
{   
    ivec_t panel_size = {32, 32};
    ivec_t button_size = {panel_size.x - 20, 40};
    //float height_inc = 1.0 / 5.0;
    ivec_t button_base_pos = IVEC_ZERO;
    ui_entity_t panel = 
    {
        .base_align = {0, 0.5},
        .base_pos = {20, 0},
        .size = panel_size,
        .is_visible = 1,
        .is_mouse_shadowing = 1,
        .type = UI_TYPE_PANEL_9_PATCH,
        .parent_align = {0, 0.5},
        .panel_9_patch = 
        {
            .left = 5,
            .top = 5,
            .right = 5,
            .bottom = 5,
            .tex_id = TEXTURE_ID_BUTTON_1,
            .tint = WHITE,
        }
    };

    ui_id_t panel_id = ui_add_child(ui, NULL, &panel);

    return panel_id;
}

// TODO: need to be finished
void ui_adjust_sizes(ui_system_t* ui_sys, int screen_w, int screen_h)
{
}
// TODO: need to be finished
void ui_init_system(ui_system_t* ui_sys)
{
    ui_sys->ui = ui_allocate(8, 32);
    ui_sys->main_menu_id = ui_add_main_menu(&ui_sys->ui, NULL);
    //ui_sys0>bottom_panel_id = ui_add_ingame_bottom_panel(&ui_sys->ui, NULL);
}

void ui_process_system(ui_system_t* ui_sys, app_state_t* app_state, int screen_w, int screen_h)
{
    ui_adjust_sizes(ui_sys, screen_w, screen_h);
    ui_sys->is_mouse_over_map = !ui_process(&ui_sys->ui, screen_w, screen_h);
}

void ui_destroy_system(ui_system_t* ui_sys)
{
    ui_free(&ui_sys->ui);
}

#endif // UI_SYSTEM_H
