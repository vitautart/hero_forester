#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include "common.h"
#include <uilib.h>

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

ui_id_t ui_add_child_text_button(ui_t* ui, ui_id_t* entity, const char* text,
        ivec_t size, ivec_t base_pos, Vector2 base_align, Vector2 parent_align, button_cb on_click)
{
    ui_entity_t panel = 
    {
        .base_align = base_align,
        .base_pos = base_pos,
        .size = size,
        .is_visible = 1,
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

#endif // UI_SYSTEM_H
