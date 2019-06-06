use code_test_lib as ct;
use ct::{ behavior, prelude::*, player };
use std::f32::consts;

const AVOID_DISTANCE: f32 = 1.0;
const MOVE_SPEED: f32 = 5.0;
const ROTATION_SPEED: f32 = 3.0;
const DEACCELERATION: f32 = 0.5;

pub struct EnemyShipBehavior {
    scan_result: behavior::SensorScanResult,
    scan_directions: [Vector2; behavior::SENSOR_RESOLUTION],
    aim_timer: f32
}

fn vec_mag(v: &Vector2) -> f32 {
    v.dot(v).sqrt()
}

impl EnemyShipBehavior {
    pub fn new() -> Self {
        let mut scan_directions: [Vector2; behavior::SENSOR_RESOLUTION] = [Vector2::zeros(); behavior::SENSOR_RESOLUTION];

        let radians_offset = consts::PI * 2.0 / behavior::SENSOR_RESOLUTION as f32;
        let mut radians = 0.0f32;
        for i in 0..behavior::SENSOR_RESOLUTION {
            scan_directions[i] = Vector2::new(radians.cos(), radians.sin());
            radians += radians_offset;
        }

        Self {
            scan_result: behavior::SensorScanResult::default(),
            scan_directions,
            aim_timer: 0.0
        } 
    }
}

impl behavior::ShipBehavior for EnemyShipBehavior {
    fn behave(
        &mut self,
        ship: &behavior::BehaviorShipInfo,
        _game: &behavior::BehaviorGameInfo,
        dt: f32,
        api: &mut behavior::ShipBehaviorApi,
    ) {
        let mut closest_asteroid: Option<Point2> = None;
        let mut closest_asteroid_distance = 999.0;

        // Check scan results from last frame
        let scan_results_last_frame = self.scan_result.latest();
        for (idx, opt) in scan_results_last_frame.hits.iter().enumerate() {
            if opt.is_some() {
                let ray_hit = opt.unwrap();

                // Ignore the player
                if let ct::raycast::RayHitKind::Ship = ray_hit.kind { continue; }

                let origin = scan_results_last_frame.origin;
                let dir = scan_results_last_frame.directions[idx];
                let hit_world_pos = origin + dir * ray_hit.t;

                let to_hit = hit_world_pos - ship.position;
                let distance_to_hit = vec_mag(&to_hit);

                if distance_to_hit < closest_asteroid_distance {
                    closest_asteroid = Some(hit_world_pos);
                    closest_asteroid_distance = distance_to_hit;
                }
            }
        }

        // Origo is default target
        let mut target = Point2::origin();

        if closest_asteroid.is_some() {
            target = closest_asteroid.unwrap();
            if ship.spin < 0.03 {
                self.aim_timer += dt;
            }
        } else {
            self.aim_timer = 0.0;
        }

        let mut thrust = Vector2::zeros();
        let to_target = target - ship.position;
        let avoid_distance_scaler = 1.0 - vec_mag(&ship.position.coords) / AVOID_DISTANCE;

        if vec_mag(&to_target) < AVOID_DISTANCE * avoid_distance_scaler.max(0.0) {
            thrust = to_target * dt * MOVE_SPEED;
        }

        let torque = player::calculate_player_ship_torque_for_aim(to_target, ship.rotation, ship.spin);
        api.set_torque(torque * 0.5 - ship.spin * ROTATION_SPEED);
        api.set_thrust(thrust + ship.velocity * DEACCELERATION);

        if self.aim_timer > 1.0 {
            api.shoot();
            self.aim_timer = 0.0;
        }
        
        api.scan(&self.scan_directions, &self.scan_result);
    }
}
