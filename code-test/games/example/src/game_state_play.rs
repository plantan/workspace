use code_test_lib as ct;
use ct::prelude::*;
use ct::gfx;
use std::f32::consts;
use rand::Rng;

use super::audio::{ AudioRequester, AudioRequest };
use super::raycast;
use super::collision;
use super::ship_factory;
use super::projectile;
use super::game_state;

pub struct GameStatePlay {
    player_input_tx: SyncSender<ct::player::PlayerInput>,

    ship_factory: ship_factory::ShipFactory,
    ship_draw_data: Vec<gfx::ShipDrawData>,

    raycast_processor: raycast::RaycastProcessor,
    collision_system: collision::CollisionSystem,
    pressed_shoot_prev_frame: bool,

    projectile_shooter: projectile::ProjectileShooter,
    asteroid_spawn_timer: f32,

    gfx_util: ct::gfx::GfxUtil,
    ship_behavior_processor: ct::behavior::ShipBehaviorProcessor,
}

impl GameStatePlay {
    pub fn new(ctx: &mut Context) -> Self {
        let mut collision_system = collision::CollisionSystem::new();
        let mut ship_factory = ship_factory::ShipFactory::new();

        Self {
            player_input_tx: ship_factory.create_player(&mut collision_system),
            ship_factory,
            ship_draw_data: Vec::new(),
            raycast_processor: raycast::RaycastProcessor::new(),
            collision_system,
            pressed_shoot_prev_frame: false,

            projectile_shooter: projectile::ProjectileShooter::new(),
            asteroid_spawn_timer: 0.0,

            gfx_util: ct::gfx::GfxUtil::new(ctx),
            ship_behavior_processor: ct::behavior::ShipBehaviorProcessor::new(),
        }
    }

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

    fn calc_world_size(&self, ctx: &mut Context) -> Vector2 {
        let screen_size = graphics::get_size(ctx);
        let world_size = self.gfx_util.screen_to_world * na::Vector4::new(screen_size.0 as f32, screen_size.1 as f32, 0.0, 1.0);
        Vector2::new(world_size.x.abs(), world_size.y.abs())
    }   
}

impl game_state::GameState for GameStatePlay {
    fn enter(&mut self, ctx: &mut Context, audio_requester: &mut AudioRequester) {
        audio_requester.add(AudioRequest::GameplayMusic(true));
        graphics::set_background_color(ctx, graphics::Color::new(0.0, 0.0, 0.0, 1.0));
    }

    fn exit(&mut self, ctx: &mut Context, audio_requester: &mut AudioRequester) {
        audio_requester.add(AudioRequest::GameplayMusic(false));
    }

    fn update(&mut self, ctx: &mut Context, audio_requester: &mut AudioRequester, player_input: ct::player::PlayerInput, dt: f32) -> bool {
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

        self.ship_draw_data.clear();

        for i in 0..ship_count {
            // Check ship collision first. Run game over if player ship has collided!
            let collision_handle = self.ship_factory.get_ship_collision_handle(i);
            if self.collision_system.check_collider(collision_handle) {
                ship_destroy_indices.push(i);

                // Player ship has collided! Time to end state!
                if self.ship_factory.is_player(i) {
                    return true;
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

            super::wrap_position(&mut ship_info.position, &world_size, ct::SHIP_RADIUS);
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

                audio_requester.add(AudioRequest::Laser);
            }

            self.ship_draw_data.push(gfx::ShipDrawData {
                position: ship_info.position,
                rotation: ship_info.rotation,
                thrust: action.controls.thrust,
                ship_type: match self.ship_factory.is_player(i) {
                    false => gfx::DrawShipType::Enemy,
                    true => gfx::DrawShipType::Player
                }
            });
        }

        self.ship_factory.destroy_ships(&ship_destroy_indices[..]);
        self.update_asteroid_spawning(world_size, dt);
        self.update_enemy_spawning(dt);
        self.projectile_shooter.move_projectiles(world_size, &mut self.collision_system, dt);

        false
    }

    fn draw(&mut self, ctx: &mut Context) {
        graphics::clear(ctx);
        self.gfx_util.apply_view_transform(ctx);

        let (projectile_draw_data, asteroid_draw_data) = self.projectile_shooter.create_draw_data();
        self.gfx_util.draw_projectiles(ctx, projectile_draw_data.into_iter());
        self.gfx_util.draw_asteroids(ctx, asteroid_draw_data.into_iter());

        // Copy draw data by hand. Is this really the only way?
        let mut ship_draw_data = Vec::new();
        for draw_data in &self.ship_draw_data[..] {
            ship_draw_data.push(gfx::ShipDrawData { ..*draw_data });
        }
        self.gfx_util.draw_ships(ctx, ship_draw_data.into_iter());
    }
}