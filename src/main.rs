use raylib::prelude::*;

fn load_resources(rl : &mut RaylibHandle, t : &RaylibThread) -> Vec<Texture2D>
{
    let mut textures : Vec<Texture2D> = vec![];
    textures.push(rl.load_texture(t, "assets/tree.png").unwrap());
    textures.push(rl.load_texture(t, "assets/uncle.png").unwrap());
    return textures;
}

fn main() {

    let screen_w = 640;
    let screen_h = 480;
    let mut uncle_pos = rvec2( screen_w / 2, screen_h / 2);

    let window_width = screen_w;
    let window_height = screen_h;
    let (mut rl, thread) = raylib::init()
        .size(window_width, window_height)
        .title("HERO FORESTER")
        .build();

    let title : &str = "HERO FORESTER!!!";

    let textures = load_resources(&mut rl, &thread);

    
    let camera = Camera2D 
    {
        offset : uncle_pos,
        target : uncle_pos,
        rotation : 0.0,
        zoom : 3.0
    };
    rl.set_target_fps(60);
    while !rl.window_should_close()
    {
        if rl.is_key_down(raylib::consts::KeyboardKey::KEY_LEFT)
        {
            uncle_pos.x -= 1.0;
        }
        if rl.is_key_down(raylib::consts::KeyboardKey::KEY_RIGHT)
        {
            uncle_pos.x += 1.0;
        }
        if rl.is_key_down(raylib::consts::KeyboardKey::KEY_UP)
        {
            uncle_pos.y -= 1.0;
        }
        if rl.is_key_down(raylib::consts::KeyboardKey::KEY_DOWN)
        {
            uncle_pos.y += 1.0;
        }
        let mut d = rl.begin_drawing(&thread);
        d.clear_background(Color{r: 0, g: 100, b: 0, a: 255});
        d.draw_text(&title, 10, 10, 20, Color::WHITE);
        {
            let mut d1 = d.begin_mode2D(camera);
            d1.draw_texture(&textures[1], uncle_pos.x as i32, uncle_pos.y as i32, Color::WHITE);
            d1.draw_texture(&textures[0], 320 + 20, 240 + 30, Color::WHITE);
        }
    }
}
