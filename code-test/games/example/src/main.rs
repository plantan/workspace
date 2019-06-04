use code_test_lib as ct;
use code_test_lib::prelude::*;

pub mod enemy;
pub mod projectile;
pub mod collision;
mod raycast;
mod ship_factory;
mod audio;
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
}

impl MyGame {
    // I'm not that happy with this solution...
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
        let mut my_game = MyGame {
            game_state_intro: game_state::GameStateIntro,
            game_state_play: game_state_play::GameStatePlay::new(ctx),
            game_state_death: game_state::GameStateDeath,
            current_game_state_type: GameStateType::Play
        };

        my_game.get_game_state(my_game.current_game_state_type).enter(ctx);
        my_game
    }

    fn update(&mut self, ctx: &mut Context, player_input: ct::player::PlayerInput) {
        let current_game_state = self.get_game_state(self.current_game_state_type);
        if current_game_state.update(ctx, player_input) {
            current_game_state.exit(ctx);

            let (new_game_state, new_game_state_type) = match self.current_game_state_type {
                GameStateType::Intro => (&mut self.game_state_play as &mut game_state::GameState, GameStateType::Play),
                GameStateType::Play => (&mut self.game_state_death as &mut game_state::GameState, GameStateType::Death),
                GameStateType::Death => (&mut self.game_state_intro as &mut game_state::GameState, GameStateType::Intro)
            };

            new_game_state.enter(ctx);
            self.current_game_state_type = new_game_state_type;
        }
    }

    fn draw(&mut self, ctx: &mut Context) {
        self.get_game_state(self.current_game_state_type).draw(ctx);
    }
}

fn main() {
    ct::game::run::<MyGame>();
}
