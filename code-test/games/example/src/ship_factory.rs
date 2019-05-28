use code_test_lib:: { behavior, prelude::*, player, sim };
use std::collections::VecDeque;
use std::sync::mpsc::Receiver;
use super::ct::gfx;
use super::enemy;
use super::collision;

fn create_empty_ship_info() -> behavior::BehaviorShipInfo {
    behavior::BehaviorShipInfo {
        position: Point2::origin(),
        velocity: Vector2::zeros(),
        rotation: 0.0,
        spin: 0.0
    }
}

fn copy_ship_info(ship_behavior_info: &behavior::BehaviorShipInfo) -> behavior::BehaviorShipInfo {
    behavior::BehaviorShipInfo {
        position: ship_behavior_info.position,
        velocity: ship_behavior_info.velocity,
        rotation: ship_behavior_info.rotation,
        spin: ship_behavior_info.spin,
    }
}

pub struct ShipFactory {
    player: (behavior::BehaviorShipInfo, player::PlayerShipBehavior),
    enemies: Vec<(behavior::BehaviorShipInfo, enemy::EnemyShipBehavior)>
    // collision_handles
}

impl ShipFactory {
    pub fn new(input_channel: Receiver<player::PlayerInput>) -> Self {
        Self {
            player: (create_empty_ship_info(), player::PlayerShipBehavior::new(input_channel)),
            enemies: Vec::with_capacity(10)
        }
    }

    pub fn create_enemy_ship(&mut self, collision_processor: &mut collision::CollisionSystem) {
        // let collision_handle = collision_processor.create_collider(Point2::origin(), code_test_lib::SHIP_RADIUS, 0);
        self.enemies.push((create_empty_ship_info(), enemy::EnemyShipBehavior::new()));
    }

    pub fn get_ship_info(&mut self, index: usize) -> &mut behavior::BehaviorShipInfo {
        if index == 0 {
            // Player is always at index 0
            &mut self.player.0
        } else {
            &mut self.enemies[index - 1].0
        }
    }
    
    pub fn create_draw_data(&mut self, actions: &[behavior::ShipAction]) -> Vec<gfx::ShipDrawData> {
        let mut ship_draw_data: Vec<gfx::ShipDrawData> = Vec::new();

        for i in 0..actions.len() {
            let ship_info = self.get_ship_info(i);
            let draw_data = gfx::ShipDrawData {
                position: ship_info.position,
                rotation: ship_info.rotation,
                thrust: actions[i].controls.thrust,

                // Player is always at index 0
                ship_type: if i == 0 { gfx::DrawShipType::Player } else { gfx::DrawShipType::Enemy }
            };

            ship_draw_data.push(draw_data);
        }

        ship_draw_data
    }

    pub fn update(&mut self, actions: &[behavior::ShipAction], dt: f32) {
        for i in 0..actions.len() {
            let ship_info = &mut self.get_ship_info(i);

            let action = &actions[i];
            sim::simulate_ship(
                action.controls,
                dt,
                &mut ship_info.position, 
                &mut ship_info.velocity,
                &mut ship_info.rotation,
                &mut ship_info.spin,
            );
        }
    }
}

pub struct ShipFactoryIterator<'a> {
    pub ship_queue: VecDeque<(behavior::BehaviorShipInfo, &'a mut behavior::ShipBehavior)>
}

impl<'a> ShipFactoryIterator<'a> {
    pub fn new(ship_factory: &'a mut ShipFactory) -> Self {
        let mut ship_queue = VecDeque::with_capacity(ship_factory.enemies.len() + 1);

        let player_copy = copy_ship_info(&ship_factory.player.0);
        ship_queue.push_back((player_copy, &mut ship_factory.player.1 as &mut behavior::ShipBehavior));

        for (ship_info, behavior) in &mut ship_factory.enemies[..] {
            let copy = copy_ship_info(ship_info);
            ship_queue.push_back((copy, &mut(*behavior) as &mut behavior::ShipBehavior));
        }

        Self {
            ship_queue
        }
    }
}

impl<'a> Iterator for ShipFactoryIterator<'a> {
    type Item = (behavior::BehaviorShipInfo, &'a mut behavior::ShipBehavior);
    
    fn next(&mut self) -> std::option::Option<Self::Item> {
        match self.ship_queue.pop_front() {
            None => None,
            Some(x) => Some((x.0, &mut(*x.1)))
        }
    }
}
