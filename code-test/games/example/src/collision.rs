use code_test_lib::prelude::*;

struct Collider {
    position: Point2,
    radius: f32,
    team: u32
}

pub struct CollisionProcessor {
    colliders: Vec<Collider>
}

impl CollisionProcessor {
    pub fn new() -> Self {
        Self {
            colliders: Vec::new()
        } 
    }

    pub fn create_collider(&mut self, position: Point2, radius: f32, team: u32) -> usize {
        let collider = Collider {
            position,
            radius,
            team
        };

        self.colliders.push(collider);
        self.colliders.len() - 1
    }

    /// Returns false if move resulted in collision
    pub fn get_state(&self, handle: usize) -> bool {
        false
    }

    pub fn move_collider(&self, handle: usize, new_position: Point2) {

    }
}
