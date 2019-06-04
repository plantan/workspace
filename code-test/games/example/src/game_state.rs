use code_test_lib as ct;
use code_test_lib::prelude::*;
use super::audio;

pub trait GameState {
    fn enter(&mut self, ctx: &mut Context);
    fn exit(&mut self, ctx: &mut Context);
    fn update(&mut self, ctx: &mut Context, player_input: ct::player::PlayerInput) -> bool;
    fn draw(&mut self, ctx: &mut Context);
}

const GAME_OVER_DURATION: f32 = 3.0;

pub struct GameStateIntro;

impl GameStateIntro {

}

impl GameState for GameStateIntro {
    fn enter(&mut self, ctx: &mut Context) {
        audio::play_music();
        graphics::set_background_color(ctx, graphics::Color::new(0.0, 1.0, 0.0, 1.0));
    }

    fn exit(&mut self, ctx: &mut Context) {

    }

    fn update(&mut self, ctx: &mut Context, player_input: ct::player::PlayerInput) -> bool {
        false
    }

    fn draw(&mut self, ctx: &mut Context) {
        graphics::clear(ctx);
    }
}

pub struct GameStateDeath;

impl GameStateDeath {

}

impl GameState for GameStateDeath {
    fn enter(&mut self, ctx: &mut Context) {
        graphics::set_background_color(ctx, graphics::Color::new(1.0, 0.0, 0.0, 1.0));
    }

    fn exit(&mut self, ctx: &mut Context) {

    }

    fn update(&mut self, ctx: &mut Context, player_input: ct::player::PlayerInput) -> bool {
        // let new_time = self.game_over_timer + dt;
        // self.game_over_timer = (self.game_over_timer + dt).min(GAME_OVER_DURATION);

        // if new_time > GAME_OVER_DURATION { self.reset(ctx); }
        false
    }

    fn draw(&mut self, ctx: &mut Context) {
        graphics::clear(ctx);
    }
}
