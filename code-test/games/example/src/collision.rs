use code_test_lib::prelude::*;

// #[derive(Clone)]
// struct Rect {
//     tl: Point2,
//     tr: Point2,
//     bl: Point2,
//     br: Point2
// }

#[derive(Clone)]
pub struct Collider {
    pub position: Point2,
    pub radius: f32,
    team: u32,
    has_collided: bool
}

pub struct CollisionSystem {
    pub colliders: Vec<Collider>
}

impl CollisionSystem {
    pub fn new() -> Self {
        Self {
            colliders: Vec::new()
        } 
    }

    pub fn create_collider(&mut self, position: Point2, radius: f32, team: u32) -> usize {
        let collider = Collider {
            position,
            radius,
            team,
            has_collided: false
        };

        self.colliders.push(collider);
        self.colliders.len() - 1
    }

    pub fn release_collider(&self, handle: usize) {
        // Return collision handle
    }

    pub fn check_collider(&self, handle: usize) -> bool {
        self.colliders[handle].has_collided
    }

    pub fn update_collider(&mut self, handle: usize, position: Point2, radius: f32) {
        let collider = &mut self.colliders[handle];
        collider.position = position;
        collider.radius = radius;
    }

    pub fn test_collisions_and_cache_result(&mut self) {
        let mut colliders_clone = self.colliders.clone();

        for collider in &mut self.colliders[..] {
            // We only need to know if a single collision has occured for a collider
            if collider.has_collided { continue; }

            colliders_clone.pop();
            for i in (0..colliders_clone.len()).rev() {
                let mut collider_to_test_against = &mut colliders_clone[i];

                // Collisions can't occur for colliders on the same "team"
                if collider.team == collider_to_test_against.team { continue; }

                let delta = collider.position - collider_to_test_against.position;
                let radius_sum = collider.radius + collider_to_test_against.radius;

                if delta.dot(&delta) < radius_sum * radius_sum {
                    collider.has_collided = true;
                    collider_to_test_against.has_collided = true;
                    break;
                }
            }
        }
    }
}
