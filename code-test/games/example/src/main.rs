use code_test_lib as ct;
use code_test_lib::prelude::*;
use std::f32::consts;
use rand::Rng;

pub mod enemy;
pub mod projectile;
pub mod collision;
mod raycast;
mod ship_factory;
mod audio;

const GAME_OVER_DURATION: f32 = 3.0;

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

struct MyGame {
    // std::sync::mpsc::SyncSender for player input. PlayerShipBehavior receives it every frame.
    player_input_tx: SyncSender<ct::player::PlayerInput>,

    ship_factory: ship_factory::ShipFactory,
    raycast_processor: raycast::RaycastProcessor,
    collision_system: collision::CollisionSystem,
    pressed_shoot_prev_frame: bool,

    projectile_shooter: projectile::ProjectileShooter,
    asteroid_spawn_timer: f32,

    // Utilities for drawing the ship.
    gfx_util: ct::gfx::GfxUtil,

    // Necessary to evaluate ship behaviors, even the player's.
    ship_behavior_processor: ct::behavior::ShipBehaviorProcessor,

    is_game_over: bool,
    game_over_timer: f32
}

impl MyGame {
    fn update_asteroid_spawning(&mut self, world_size: Vector2, dt: f32) {
        self.asteroid_spawn_timer -= dt;
        if self.asteroid_spawn_timer < 0.0 {
            self.asteroid_spawn_timer = 1.0;

            let mut rng = rand::thread_rng();
            let r = rng.gen_range(0.0, consts::PI * 2.0) as f32;

            let spawn = Point2::new(r.cos() * world_size.x, r.sin() * world_size.y);
            let target = Point2::new(rng.gen_range(0.0, world_size.x * 0.5), rng.gen_range(0.0, world_size.y * 0.5));
            let velocity = (target - spawn).normalize() * rng.gen_range(0.05, 0.3);
            self.projectile_shooter.fire(spawn, velocity, r, projectile::ProjectileType::Asteroid(false), &mut self.collision_system);
        }
    }

    fn update_enemy_spawning(&mut self, dt: f32) {
        
    }

    fn reset(&mut self, ctx: &mut Context) {
        self.is_game_over = false;
        self.game_over_timer = 0.0;

        self.player_input_tx = self.ship_factory.create_player(&mut self.collision_system);
        graphics::set_background_color(ctx, graphics::Color::new(0.0, 0.0, 0.0, 1.0));
    }

    fn calc_world_size(&self, ctx: &mut Context) -> Vector2 {
        let screen_size = graphics::get_size(ctx);
        let world_size = self.gfx_util.screen_to_world * na::Vector4::new(screen_size.0 as f32, screen_size.1 as f32, 0.0, 1.0);
        Vector2::new(world_size.x.abs(), world_size.y.abs())
    }   
}

// Main entry points for the Code Test game.
impl ct::game::CodeTestImpl for MyGame {
    fn new(ctx: &mut Context) -> Self {
        audio::play_music();
        graphics::set_background_color(ctx, graphics::Color::new(0.0, 0.0, 0.0, 1.0));

        let mut collision_system = collision::CollisionSystem::new();
        let mut ship_factory = ship_factory::ShipFactory::new();

        Self {
            player_input_tx: ship_factory.create_player(&mut collision_system),
            ship_factory,
            raycast_processor: raycast::RaycastProcessor::new(),
            collision_system,
            pressed_shoot_prev_frame: false,

            projectile_shooter: projectile::ProjectileShooter::new(),
            asteroid_spawn_timer: 0.0,

            gfx_util: ct::gfx::GfxUtil::new(ctx),
            ship_behavior_processor: ct::behavior::ShipBehaviorProcessor::new(),

            is_game_over: false,
            game_over_timer: 0.0
        }
    }

    fn update(&mut self, ctx: &mut Context, player_input: ct::player::PlayerInput) {
        // Duration::as_float_secs is unstable, so we calculate it ourselves
        let dt: f32 = timer::get_delta(ctx).subsec_nanos().min(100_000_000) as f32 * 1e-9;

        ////////////////////////////////////////////////////////////////////////////////////////////

        if self.is_game_over {
            let new_time = self.game_over_timer + dt;
            self.game_over_timer = (self.game_over_timer + dt).min(GAME_OVER_DURATION);

            if new_time > GAME_OVER_DURATION { self.reset(ctx); }

            // Perform early out if we're in game over state
            return;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////

        // Center the camera around the origin, and calculate its transformations.
        // We need them here for mouse aim.
        self.gfx_util.calculate_view_transform(ctx, Point2::origin(), 1.0);

        // Override input to LMB since it seems to return true for
        // every frame, and not only for the LMB down event
        let mut player_input_override = player_input.clone();
        player_input_override.shoot = player_input_override.shoot && !self.pressed_shoot_prev_frame;
        self.pressed_shoot_prev_frame = player_input.shoot;
        
        // Use the calculated camera transformation to find the world position
        // of the mouse cursor, and send the input to the player ship behavior.
        self.player_input_tx
            .send(self.gfx_util.screen_to_world(player_input_override))
            .ok();

        self.ship_behavior_processor.run_behaviors(
            ship_factory::ShipFactoryIterator::new(&mut self.ship_factory),
            &ct::behavior::BehaviorGameInfo { safe_radius: 1.0 },
            dt
        );

        let world_size = self.calc_world_size(ctx);

        // Evaluate collision and visibility raycasts
        self.collision_system.test_colliders();
        self.ship_behavior_processor.process_raycasts(&self.raycast_processor);
        self.raycast_processor.clear_targets();

        // There's always one actions for each ship
        let actions = &self.ship_behavior_processor.get_actions();
        let ship_count = self.ship_factory.get_ship_count();
        let mut ship_destroy_indices = Vec::new();

        for i in 0..ship_count {
            // Check ship collision first. Run game over if player ship has collided!
            let collision_handle = self.ship_factory.get_ship_collision_handle(i);
            if self.collision_system.check_collider(collision_handle) {
                ship_destroy_indices.push(i);

                if self.ship_factory.is_player(i) {
                    self.is_game_over = true;
                }

                continue;
            }

            let ship_info = self.ship_factory.get_ship_info(i);
            let action = &actions[i];

            ct::sim::simulate_ship(
                action.controls,
                dt,
                &mut ship_info.position, 
                &mut ship_info.velocity,
                &mut ship_info.rotation,
                &mut ship_info.spin,
            );

            wrap_position(&mut ship_info.position, &world_size, ct::SHIP_RADIUS);
            self.collision_system.update_collider(collision_handle, ship_info.position, ct::SHIP_RADIUS);
            self.raycast_processor.add_target(ship_info.position, ct::SHIP_RADIUS);

            if action.shoot {
                let rotation = ship_info.rotation + consts::PI * 0.5;
                let velocity = Vector2::new(rotation.cos(), rotation.sin()).normalize() * 3.5;
                self.projectile_shooter.fire(ship_info.position, 
                    velocity, 
                    rotation + consts::PI * 0.5,
                    projectile::ProjectileType::Laser, 
                    &mut self.collision_system
                );
            }
        }

        self.ship_factory.destroy_ships(&ship_destroy_indices[..]);
        self.update_asteroid_spawning(world_size, dt);
        self.update_enemy_spawning(dt);
        
        self.projectile_shooter.move_projectiles(world_size, &mut self.collision_system, dt);
    }

    fn draw(&mut self, ctx: &mut Context) {
        graphics::clear(ctx);
        self.gfx_util.apply_view_transform(ctx);

        if self.is_game_over {
            let redness = 1.0 - self.game_over_timer / GAME_OVER_DURATION;
            graphics::set_background_color(ctx, graphics::Color::new(redness * redness, 0.0, 0.0, 1.0));
        } else {
            for c in &self.collision_system.colliders[..] {
                graphics::circle(ctx, DrawMode::Fill, c.position, c.radius, 100.0).ok();
            }

            let (projectile_draw_data, asteroid_draw_data) = self.projectile_shooter.create_draw_data();
            self.gfx_util.draw_projectiles(ctx, projectile_draw_data.into_iter());
            self.gfx_util.draw_asteroids(ctx, asteroid_draw_data.into_iter());

            let actions = &self.ship_behavior_processor.get_actions();
            let draw_data_iter = self.ship_factory.create_draw_data(actions).into_iter();
            self.gfx_util.draw_ships(ctx, draw_data_iter);
        }
    }
}

fn main() {
    ct::game::run::<MyGame>();
}
