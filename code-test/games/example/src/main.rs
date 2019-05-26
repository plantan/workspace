use code_test_lib as ct;
use code_test_lib::prelude::*;
use std::f32::consts;

pub mod enemy;
pub mod collision;
pub mod projectile;
mod raycast;
mod ship_factory;

struct MyGame {
    // std::sync::mpsc::SyncSender for player input. PlayerShipBehavior receives it every frame.
    player_input_tx: SyncSender<ct::player::PlayerInput>,

    ship_factory: ship_factory::ShipFactory,
    projectile_shooter: projectile::ProjectileShooter,
    raycast_processor: raycast::RaycastProcessor,

    // Utilities for drawing the ship.
    gfx_util: ct::gfx::GfxUtil,

    // Necessary to evaluate ship behaviors, even the player's.
    ship_behavior_processor: ct::behavior::ShipBehaviorProcessor,
}

// Main entry points for the Code Test game.
impl ct::game::CodeTestImpl for MyGame {
    fn new(ctx: &mut Context) -> Self {
        let (player_input_tx, player_input_rx) = sync_channel(1);
        let mut ship_factory = ship_factory::ShipFactory::new(player_input_rx);
        ship_factory.create_enemy_ship();

        Self {
            player_input_tx,
            ship_factory,
            projectile_shooter: projectile::ProjectileShooter::new(),
            raycast_processor: raycast::RaycastProcessor::new(),
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

        // Use the calculated camera transformation to find the word position
        // of the mouse cursor, and send the input to the player ship behavior.
        self.player_input_tx
            .send(self.gfx_util.screen_to_world(player_input))
            .ok();

        self.ship_behavior_processor.run_behaviors(
            ship_factory::ShipFactoryIterator::new(&mut self.ship_factory),
            &ct::behavior::BehaviorGameInfo { safe_radius: 1.0 },
            dt,
        );

        // Once behaviors have executed, evaluate their visibility raycasts,
        // as they will be needed in the next frame.
        self.ship_behavior_processor.process_raycasts(&self.raycast_processor);
        self.raycast_processor.clear_targets();
        
        let actions = &self.ship_behavior_processor.get_actions();
        for i in 0..actions.len() {
            let ship_info = &mut self.ship_factory.get_ship_info(i);

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
                let rotation_offset = consts::PI * 0.5;
                self.projectile_shooter.fire(ship_info.position, ship_info.rotation + rotation_offset, projectile::ProjectileType::Projectile);
            }

            self.raycast_processor.add_target(ship_info.position, ct::SHIP_RADIUS);
        }

        self.projectile_shooter.update(dt);
    }

    fn draw(&mut self, ctx: &mut Context) {
        graphics::clear(ctx);
        self.gfx_util.apply_view_transform(ctx);

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
