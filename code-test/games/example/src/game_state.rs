use code_test_lib as ct;
use super::audio::{ AudioRequester, AudioRequest };
use ct::prelude::*;
use graphics::{ self, Text };

pub trait GameState {
    fn enter(&mut self, _:&mut Context, _:&mut AudioRequester);
    fn exit(&mut self, _:&mut Context, _:&mut AudioRequester);
    fn update(&mut self, _:&mut Context, _:&mut AudioRequester, _:ct::player::PlayerInput, dt: f32) -> bool;
    fn draw(&mut self, _:&mut Context);
}

const GAME_OVER_DURATION: f32 = 3.0;
const FADE_INTO_GAMEPLAY_DURATION: f32 = 3.0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Intro State

pub struct GameStateIntro {
    start_game_text: Text,
    good_luck_text: Text,
    start_game_text_blink: f32,
    fade_into_gameplay_timer: f32,
    has_pressed_confirm: bool
}

impl GameStateIntro {
    pub fn new(ctx: &mut Context) -> Self {
        Self {
            start_game_text: Text::new(ctx, "PRESS LMB TO START GAME!", &graphics::Font::default_font().unwrap()).unwrap(),
            good_luck_text: Text::new(ctx, "GOOD LUCK!", &graphics::Font::default_font().unwrap()).unwrap(),
            start_game_text_blink: 0.0,
            fade_into_gameplay_timer: 0.0,
            has_pressed_confirm: false
        }
    }
}

impl GameState for GameStateIntro {
    fn enter(&mut self, ctx: &mut Context, audio_requester: &mut AudioRequester) {
        // Reset view transform
        graphics::set_transform(ctx, Matrix4::identity());

        audio_requester.add(AudioRequest::IntroMusic(true));
        self.has_pressed_confirm = false;
        self.fade_into_gameplay_timer = FADE_INTO_GAMEPLAY_DURATION;
        self.start_game_text_blink = 0.0;
    }

    fn exit(&mut self, _ctx: &mut Context, audio_requester: &mut AudioRequester) {
        
    }

    fn update(&mut self, ctx: &mut Context, audio_requester: &mut AudioRequester, player_input: ct::player::PlayerInput, dt: f32) -> bool {
        if player_input.shoot && !self.has_pressed_confirm {
            self.has_pressed_confirm = true;
            audio_requester.add(AudioRequest::IntroMusic(false));
            audio_requester.add(AudioRequest::Confirm);
        }

        // I miss the ternary operator...
        let dt = if self.has_pressed_confirm { dt } else { 0.0 };
        self.start_game_text_blink += dt * 20.0;
        self.fade_into_gameplay_timer -= dt;

        // End the state if we have finished fading
        self.fade_into_gameplay_timer < 0.0
    }

    fn draw(&mut self, ctx: &mut Context) {
        let p = self.fade_into_gameplay_timer / FADE_INTO_GAMEPLAY_DURATION * 0.6;
        let p = p * p; // Make a curve
        graphics::set_background_color(ctx, graphics::Color::new(p, p, p, 1.0));
        graphics::clear(ctx);

        if self.start_game_text_blink.cos() > 0.0 {
            let screen_size = graphics::get_size(ctx);
            let text_pos = Point2::new(screen_size.0 as f32 * 0.5, screen_size.1 as f32 * 0.5);
            let p = graphics::DrawParam {
                dest: text_pos,
                scale: graphics::Point2::new(2.0, 1.0),
                offset: Point2::new(0.5, 0.5),
                rotation: 0.0,
                ..Default::default()
            };

            let text = if self.has_pressed_confirm {
                &self.good_luck_text
            } else {
                &self.start_game_text
            };

            graphics::draw_ex(ctx, text, p).ok();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Death State

pub struct GameStateDeath {
    game_over_timer: f32
}

impl GameStateDeath {
    pub fn new() -> Self {
        Self {
            game_over_timer: 0.0
        }
    }
}

impl GameState for GameStateDeath {
    fn enter(&mut self, ctx: &mut Context, audio_requester: &mut AudioRequester) {
        // Reset view transform
        graphics::set_transform(ctx, Matrix4::identity());

        audio_requester.add(AudioRequest::Death);
        self.game_over_timer = GAME_OVER_DURATION;
    }

    fn exit(&mut self, _ctx: &mut Context, audio_requester: &mut AudioRequester) {

    }

    fn update(&mut self, _ctx: &mut Context, audio_requester: &mut AudioRequester, player_input: ct::player::PlayerInput, dt: f32) -> bool {
        self.game_over_timer -= dt;
        self.game_over_timer < 0.0
    }

    fn draw(&mut self, ctx: &mut Context) {
        let p = self.game_over_timer / GAME_OVER_DURATION;
        graphics::set_background_color(ctx, graphics::Color::new(p * p, 0.0, 0.0, 1.0));
        graphics::clear(ctx);
    }
}

