use code_test_lib as ct;
use ct::{ behavior, prelude::*, player };
use std::f32::consts;
use lerp::Lerp;

const AVOID_DISTANCE: f32 = 0.1;
const MOVE_SPEED: f32 = 3.0;
const ROTATION_SPEED: f32 = 3.0;
const DEACCELERATION: f32 = 0.5;

fn vec_mag(v: &Vector2) -> f32 {
    v.dot(v).sqrt()
}

fn rad_to_dir(rad: f32) -> Vector2 {
    let rad_with_offset = rad - consts::PI * 0.5;
    -Vector2::new(rad_with_offset.cos(), rad_with_offset.sin())
}

fn gen_scan_directions(start_rad: f32) -> [Vector2; behavior::SENSOR_RESOLUTION] {
    let mut scan_directions: [Vector2; behavior::SENSOR_RESOLUTION] = [Vector2::zeros(); behavior::SENSOR_RESOLUTION];

    let radians_offset = consts::PI * 2.0 / behavior::SENSOR_RESOLUTION as f32;
    let mut radians = start_rad;
    for i in 0..behavior::SENSOR_RESOLUTION {
        scan_directions[i] = Vector2::new(radians.cos(), radians.sin());
        radians += radians_offset;
    }

    scan_directions
}

fn get_closest_ray_hit(scan_result: &mut ct::behavior::SensorScanResult) -> Option<Point2> {
    let mut closest_hit: Option<Point2> = None;
    let mut closest_hit_distance = 999.0;

    // Check scan results from last frame
    let scan_results_last_frame = scan_result.latest();
    for (idx, opt) in scan_results_last_frame.hits.iter().enumerate() {
        if opt.is_some() {
            let ray_hit = opt.unwrap();

            // Ignore the player
            if let ct::raycast::RayHitKind::Ship = ray_hit.kind { continue; }

            let origin = scan_results_last_frame.origin;
            let dir = scan_results_last_frame.directions[idx];
            let hit_world_pos = origin + dir * ray_hit.t;

            let to_hit = hit_world_pos - scan_results_last_frame.origin;
            let distance_to_hit = vec_mag(&to_hit);

            if distance_to_hit < closest_hit_distance {
                closest_hit = Some(hit_world_pos);
                closest_hit_distance = distance_to_hit;
            }
        }
    }

    closest_hit
}

pub struct EnemyShipBehavior {
    scan_result: behavior::SensorScanResult,
    target: Point2,
    target_valid_timer: f32,
    scan_offset: f32,
    has_ammo: bool
}

impl EnemyShipBehavior {
    pub fn new() -> Self {
        Self {
            scan_result: behavior::SensorScanResult::default(),
            target: Point2::origin(),
            target_valid_timer: 0.0,
            scan_offset: 0.0,
            has_ammo: false
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
        let closest_ray_hit = get_closest_ray_hit(&mut self.scan_result);

        if self.target_valid_timer > 0.0 {
            self.target_valid_timer -= dt;
            let to_target = self.target - ship.position;

            // Aim
            let mut torque = player::calculate_player_ship_torque_for_aim(to_target, ship.rotation, ship.spin);
            torque = torque * 0.5 - ship.spin * ROTATION_SPEED;
            api.set_torque(torque);

            // Keep distance
            let move_target = ship.position + to_target.normalize() * AVOID_DISTANCE;
            let thrust = move_target - ship.position * dt * MOVE_SPEED;
            api.set_thrust(thrust + ship.velocity * DEACCELERATION);

            // Require better aim depending on distance to target
            let aim_angle = 0.9_f32.lerp(0.999, (vec_mag(&to_target) / 0.5).min(1.0));

            // Good enough aim to fire?
            if to_target.normalize().dot(&rad_to_dir(ship.rotation)) > aim_angle && self.has_ammo {
                api.shoot();
                self.has_ammo = false;
            }
        } else if closest_ray_hit.is_some() {
            // We found a new target!
            self.target_valid_timer = 1.0;
            self.target = closest_ray_hit.unwrap();
            self.has_ammo = true;
        } else {
            // Idle behavior (do nothing)
        }

        self.scan_offset += dt * consts::PI * 2.0;
        let scan_directions = gen_scan_directions(self.scan_offset);
        api.scan(&scan_directions, &self.scan_result);
    }
}
