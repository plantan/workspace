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
}

impl MyGame {
    fn update_asteroid_spawning(&mut self, dt: f32) {
        self.asteroid_spawn_timer -= dt;
        if self.asteroid_spawn_timer < 0.0 {
            self.asteroid_spawn_timer = 1.0;

            let mut rng = rand::thread_rng();
            let r = rng.gen_range(0.0, consts::PI * 2.0) as f32;

            let random_pos = Point2::new(r.cos(), r.sin()) * rng.gen_range(1.0, 2.0) as f32;
            let random_target = Point2::new(rng.gen_range(-1.0, 1.0), rng.gen_range(-1.0, 1.0));
            let velocity = (random_target - random_pos).normalize() * rng.gen_range(0.05, 0.3);
            self.projectile_shooter.fire(random_pos, velocity, r, projectile::ProjectileType::Asteroid, &mut self.collision_system);
        }
    }
}

// Main entry points for the Code Test game.
impl ct::game::CodeTestImpl for MyGame {
    fn new(ctx: &mut Context) -> Self {
        let mut collision_system = collision::CollisionSystem::new();
        let mut ship_factory = ship_factory::ShipFactory::new();
        ship_factory.create_enemy(&mut collision_system);

        audio::play_music();

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
        }
    }

    fn update(&mut self, ctx: &mut Context, player_input: ct::player::PlayerInput) {
        // Duration::as_float_secs is unstable, so we calculate it ourselves
        let dt: f32 = timer::get_delta(ctx).subsec_nanos().min(100_000_000) as f32 * 1e-9;

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

        self.collision_system.test_colliders();

        // Once behaviors have executed, evaluate their visibility raycasts,
        // as they will be needed in the next frame.
        self.ship_behavior_processor.process_raycasts(&self.raycast_processor);
        let actions = &self.ship_behavior_processor.get_actions();
        self.raycast_processor.clear_targets();

        for i in 0..actions.len() {
            let collision_handle = self.ship_factory.get_ship_collision_handle(i);
            if self.collision_system.check_collider(collision_handle) {
                self.ship_factory.destroy_ship(i);
                continue;
            }

            // Access ship info with same index as for actions, since there's one action for each ship
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

            if action.shoot {
                let rotation = ship_info.rotation + consts::PI * 0.5;
                let velocity = Vector2::new(rotation.cos(), rotation.sin()).normalize() * 3.5;
                self.projectile_shooter.fire(ship_info.position, 
                    velocity, 
                    rotation, 
                    projectile::ProjectileType::Laser, 
                    &mut self.collision_system
                );
            }

            self.raycast_processor.add_target(ship_info.position, ct::SHIP_RADIUS);
        }
        
        self.update_asteroid_spawning(dt);
        self.projectile_shooter.update(graphics::get_size(ctx), 
            self.gfx_util.world_to_screen, 
            &mut self.collision_system, 
            dt
        );
    }

    fn draw(&mut self, ctx: &mut Context) {
        graphics::clear(ctx);
        self.gfx_util.apply_view_transform(ctx);

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

fn main() {
    ct::game::run::<MyGame>();
}
