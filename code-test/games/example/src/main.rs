use code_test_lib as ct;
use code_test_lib::prelude::*;
use std::env;
use std::path;
use audio::{ AudioPlayer, AudioRequester };

pub mod enemy;
pub mod projectile;
pub mod collision;
pub mod audio;
mod raycast;
mod ship_factory;
mod game_state;
mod game_state_play;

pub fn wrap_position(position: &mut Point2, world_size: &Vector2, offset: f32) {
    if position.x - world_size.x > offset {
        position.x = -world_size.x;
    } 
    else if position.x + world_size.x < -offset {
        position.x = world_size.x;
    }
    if position.y - world_size.y > offset {
        position.y = -world_size.y;
    } else if position.y + world_size.y < -offset {
        position.y = world_size.y;
    }
}

#[derive(Clone, Copy)]
pub enum GameStateType {
    Intro,
    Play,
    Death
}

struct MyGame {
    game_state_intro: game_state::GameStateIntro,
    game_state_play: game_state_play::GameStatePlay,
    game_state_death: game_state::GameStateDeath,
    current_game_state_type: GameStateType,

    audio_player: AudioPlayer
}

impl MyGame {
    fn get_game_state(&mut self, game_state_type: GameStateType) -> &mut game_state::GameState {
        match game_state_type {
            GameStateType::Intro => &mut self.game_state_intro as &mut game_state::GameState,
            GameStateType::Play => &mut self.game_state_play as &mut game_state::GameState,
            GameStateType::Death => &mut self.game_state_death as &mut game_state::GameState
        }
    }
}

impl ct::game::CodeTestImpl for MyGame {
    fn new(ctx: &mut Context) -> Self {
        let assets_dir = env::var("CARGO_MANIFEST_DIR").unwrap() + "\\assets";
        ctx.filesystem.mount(path::Path::new(&assets_dir), true);

        let (score_sender, score_receiver) = sync_channel(1);

        let mut my_game = MyGame {
            game_state_intro: game_state::GameStateIntro::new(ctx),
            game_state_play: game_state_play::GameStatePlay::new(ctx, score_sender),
            game_state_death: game_state::GameStateDeath::new(ctx, score_receiver),
            current_game_state_type: GameStateType::Intro,
            audio_player: AudioPlayer::new(ctx)
        };

        let mut audio_requester = AudioRequester::new();
        my_game.get_game_state(my_game.current_game_state_type).enter(ctx, &mut audio_requester);
        my_game.audio_player.play(ctx, audio_requester);

        my_game
    }

    fn update(&mut self, ctx: &mut Context, player_input: ct::player::PlayerInput) {
        // Duration::as_float_secs is unstable, so we calculate it ourselves
        let dt: f32 = timer::get_delta(ctx).subsec_nanos().min(100_000_000) as f32 * 1e-9;

        let mut audio_requester = AudioRequester::new();

        let current_game_state = self.get_game_state(self.current_game_state_type);
        if current_game_state.update(ctx, &mut audio_requester, player_input, dt) {
            current_game_state.exit(ctx, &mut audio_requester);

            let (new_game_state, new_game_state_type) = match self.current_game_state_type {
                GameStateType::Intro => (&mut self.game_state_play as &mut game_state::GameState, GameStateType::Play),
                GameStateType::Play => (&mut self.game_state_death as &mut game_state::GameState, GameStateType::Death),
                GameStateType::Death => (&mut self.game_state_intro as &mut game_state::GameState, GameStateType::Intro)
            };

            new_game_state.enter(ctx, &mut audio_requester);
            self.current_game_state_type = new_game_state_type;
        }

        self.audio_player.play(ctx, audio_requester);
    }

    fn draw(&mut self, ctx: &mut Context) {
        self.get_game_state(self.current_game_state_type).draw(ctx);
    }
}

fn main() {
    ct::game::run::<MyGame>();
}
