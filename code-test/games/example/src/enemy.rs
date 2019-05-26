use code_test_lib:: { behavior, prelude::*, player };
use std::f32::consts;

pub struct EnemyShipBehavior {
    scan_result: behavior::SensorScanResult,
    scan_directions: [Vector2; behavior::SENSOR_RESOLUTION]
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
            scan_directions
        } 
    }
}

impl behavior::ShipBehavior for EnemyShipBehavior {
    fn behave(
        &mut self,
        ship: &behavior::BehaviorShipInfo,
        _game: &behavior::BehaviorGameInfo,
        _dt: f32,
        api: &mut behavior::ShipBehaviorApi,
    ) {
        let mut point_of_interest_opt = None;

        // Check scan results from last frame
        let scan_results_last_frame = self.scan_result.latest();
        for (idx, opt) in scan_results_last_frame.hits.iter().enumerate() {
            if opt.is_some() {
                let origin = scan_results_last_frame.origin;
                let dir = scan_results_last_frame.directions[idx];
                point_of_interest_opt = Some(origin + dir * opt.unwrap().t);
                break;
            }
        }

        let mut thrust = Vector2::zeros();
        let mut torque = -ship.spin;

        if point_of_interest_opt.is_some() {
            let point_of_interest = point_of_interest_opt.unwrap();
            let to_point_of_interest = ship.position - point_of_interest;
            
            thrust = to_point_of_interest * 0.1;
            torque = player::calculate_player_ship_torque_for_aim(point_of_interest - ship.position, ship.rotation, ship.spin);
        }

        api.set_thrust(thrust + ship.velocity * 0.5);
        api.set_torque(torque - ship.spin * 5.0);

        api.shoot();
        api.scan(&self.scan_directions, &self.scan_result);
    }
}
