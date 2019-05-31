use code_test_lib::prelude::*;
use std::collections::VecDeque;

#[derive(Clone)]
pub struct Collider {
    pub position: Point2,
    pub radius: f32,
    team: u32,
    has_collided: bool
}

pub struct CollisionSystem {
    pub colliders: Vec<Collider>,
    recycle_indices: VecDeque<usize>
}

impl CollisionSystem {
    pub fn new() -> Self {
        Self {
            colliders: Vec::new(),
            recycle_indices: VecDeque::new()
        } 
    }

    pub fn create_collider(&mut self, position: Point2, radius: f32, team: u32) -> usize {
        // Assuming this allocation takes place on the stack
        let collider = Collider {
            position,
            radius,
            team,
            has_collided: false
        };

        let recycle_index = self.recycle_indices.pop_front();
        if recycle_index.is_some() {
            // Recycle array element position and return
            let recycle_index = recycle_index.unwrap();
            self.colliders[recycle_index] = collider;
            return recycle_index
        }

        self.colliders.push(collider);
        self.colliders.len() - 1
    }

    pub fn release_collider(&mut self, handle: usize) {
        self.colliders[handle].has_collided = true;
        self.recycle_indices.push_back(handle);
    }

    pub fn check_collider(&self, handle: usize) -> bool {
        self.colliders[handle].has_collided
    }

    pub fn update_collider(&mut self, handle: usize, position: Point2, radius: f32) {
        let collider = &mut self.colliders[handle];
        collider.position = position;
        collider.radius = radius;
    }

    pub fn test_colliders(&mut self) {
        let colliders_length = self.colliders.len();

        for outer_index in 0..colliders_length {
            let collider = self.colliders[outer_index].clone();

            // We only need to know if a single collision has occured for a collider
            if collider.has_collided { continue; }

            for inner_index in (outer_index + 1)..colliders_length {
                let collider_to_test_against = self.colliders[inner_index].clone();

                if collider_to_test_against.has_collided { continue; }
                if collider.team == collider_to_test_against.team { continue; }

                let delta = collider.position - collider_to_test_against.position;
                let radius_sum = collider.radius + collider_to_test_against.radius;

                if delta.dot(&delta) < radius_sum * radius_sum {
                    self.colliders[outer_index].has_collided = true;
                    self.colliders[inner_index].has_collided = true;
                    break;
                }
            }
        }
    }
}
