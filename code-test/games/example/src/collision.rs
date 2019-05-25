use code_test_lib:: { behavior, prelude::*, player };
use super::ct::gfx;
use super::enemy;

trait Collider {
    fn get_radius(&self) -> f32;
    fn get_position(&self) -> Point2;
    fn apply_collision(&self) -> bool;
}


// Implement collision traits for ships and projectiles


