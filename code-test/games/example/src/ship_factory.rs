use code_test_lib:: { behavior, prelude::*, player };
use std::collections::VecDeque;
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

pub enum BehaviorType {
    Player(player::PlayerShipBehavior),
    Enemy(enemy::EnemyShipBehavior)
}

struct ShipWrapper {
    behavior_type: BehaviorType,
    info: behavior::BehaviorShipInfo,
    collision_handle: usize
}

pub struct ShipFactory {
    ships: Vec<ShipWrapper>
}

impl ShipFactory {
    pub fn new() -> Self {
        Self {
            ships: Vec::new()
        }
    }

    pub fn create_player(&mut self, collision_system: &mut collision::CollisionSystem) -> SyncSender<code_test_lib::player::PlayerInput> {
        let (player_input_tx, player_input_rx) = sync_channel(1);
        self.create_ship(BehaviorType::Player(player::PlayerShipBehavior::new(player_input_rx)), collision_system);
        player_input_tx
    }

    pub fn create_enemy(&mut self, collision_processor: &mut collision::CollisionSystem) {
        self.create_ship(BehaviorType::Enemy(enemy::EnemyShipBehavior::new()), collision_processor);
    }

    fn create_ship(&mut self, behavior_type: BehaviorType, collision_processor: &mut collision::CollisionSystem) {
        let new_ship = ShipWrapper {
            behavior_type,
            info: create_empty_ship_info(),
            collision_handle: collision_processor.create_collider(Point2::origin(), code_test_lib::SHIP_RADIUS, 0)
        };

        self.ships.push(new_ship);
    }

    pub fn destroy_ships(&mut self, destroy_indices: &[usize]) {
        for i in destroy_indices {
            self.ships.remove(*i);
        }
    }

    pub fn get_ship_count(&mut self) -> usize {
        self.ships.len()
    }

    pub fn get_ship_info(&mut self, index: usize) -> &mut behavior::BehaviorShipInfo {
        &mut self.ships[index].info
    }

    pub fn is_player(&self, index: usize) -> bool {
        match self.ships[index].behavior_type {
            BehaviorType::Player(_) => true,
            BehaviorType::Enemy(_) => false,
        }
    }

    pub fn get_ship_collision_handle(&self, index: usize) -> usize {
        self.ships[index].collision_handle
    }

    pub fn tear_down(&mut self) {
        self.ships.clear();
    }
}

pub struct ShipFactoryIterator<'a> {
    pub ship_queue: VecDeque<(behavior::BehaviorShipInfo, &'a mut behavior::ShipBehavior)>
}

impl<'a> ShipFactoryIterator<'a> {
    pub fn new(ship_factory: &'a mut ShipFactory) -> Self {
        let mut ship_queue = VecDeque::with_capacity(ship_factory.ships.len());

        for ship in &mut ship_factory.ships[..] {
            let behavior = match &mut ship.behavior_type {
                BehaviorType::Player(player_behavior) => player_behavior as &mut behavior::ShipBehavior,
                BehaviorType::Enemy(enemy_behavior) => enemy_behavior as &mut behavior::ShipBehavior,
            };
            ship_queue.push_back((copy_ship_info(&ship.info), behavior));
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
