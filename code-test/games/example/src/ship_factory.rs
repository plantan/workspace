use code_test_lib:: { behavior, prelude::*, player };
use std::collections::VecDeque;
use super::ct::gfx;
use std::sync::mpsc::Receiver;
use super::enemy;

pub struct ShipFactory {
    ships: Vec<(behavior::BehaviorShipInfo, bool)>,
    enemy_behavior: enemy::EnemyShipBehavior,
    player_behavior: player::PlayerShipBehavior
}

impl ShipFactory {
    pub fn new(input_channel: Receiver<player::PlayerInput>) -> Self {
        Self {
            ships: Vec::with_capacity(10),
            enemy_behavior: enemy::EnemyShipBehavior::new(),
            player_behavior: player::PlayerShipBehavior::new(input_channel)
        } 
    }

    pub fn create_ship(&mut self, is_player: bool) {
        let new_ship_data = behavior::BehaviorShipInfo {
            position: Point2::origin(),
            velocity: Vector2::zeros(),
            rotation: 0.0,
            spin: 0.0
        };

        self.ships.push((new_ship_data, is_player));
    }

    pub fn get_ship_info(&mut self, index: usize) -> &mut(behavior::BehaviorShipInfo, bool) {
        &mut self.ships[index]
    }
    
    pub fn create_draw_data(&mut self, actions: &[behavior::ShipAction]) -> Vec<gfx::ShipDrawData> {
        let mut ship_draw_data: Vec<gfx::ShipDrawData> = Vec::new();

        for i in 0..actions.len() {
            let ship_info = self.get_ship_info(i);
            let ship_type = match ship_info.1 {
                true => gfx::DrawShipType::Player,
                false => gfx::DrawShipType::Enemy
            };

            let draw_data = gfx::ShipDrawData {
                position: ship_info.0.position,
                rotation: ship_info.0.rotation,
                thrust: actions[i].controls.thrust,
                ship_type,
            };

            ship_draw_data.push(draw_data);
        }

        ship_draw_data
    }
}

pub struct ShipFactoryIterator<'a> {
    pub ship_queue: VecDeque<(behavior::BehaviorShipInfo, bool)>,
    ship_factory: &'a mut ShipFactory
}

impl<'a> ShipFactoryIterator<'a> {
    pub fn new(ship_factory: &'a mut ShipFactory) -> Self {
        let mut ship_queue = VecDeque::with_capacity(ship_factory.ships.len());

        for (ship_info, is_player) in &ship_factory.ships[..] {
            let copy = behavior::BehaviorShipInfo {
                position: ship_info.position,
                velocity: ship_info.velocity,
                rotation: ship_info.rotation,
                spin: ship_info.spin,
            };

            ship_queue.push_back((copy, *is_player));
        }

        Self {
            ship_queue,
            ship_factory
        }
    }
}

impl<'a> Iterator for ShipFactoryIterator<'a> {
    type Item = (behavior::BehaviorShipInfo, &'a mut behavior::ShipBehavior);
    
    fn next(&mut self) -> std::option::Option<Self::Item> {
        match self.ship_queue.pop_front() {
            None => None,
            Some(x) => {
                let new_behavior = match x.1 {
                    true => &mut self.ship_factory.player_behavior as *mut behavior::ShipBehavior,
                    false => &mut self.ship_factory.enemy_behavior as *mut behavior::ShipBehavior
                };

                unsafe { Some((x.0, &mut(*new_behavior))) }
            }
        }
    }
}
