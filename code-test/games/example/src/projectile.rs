use code_test_lib::{ prelude::*, gfx };
use super::collision;
use std::collections::VecDeque;
use std::f32::consts;
use rand::Rng;

const LASER_LIFETIME: f32 = 1.5;
const ASTEROID_LIFETIME: f32 = 999.0;

fn generate_projectile_radius(projectile_type: &ProjectileType) -> f32 {
    match projectile_type {
        ProjectileType::Laser => 0.05,
        ProjectileType::Asteroid(is_split) => {
            let (min, max) = match is_split {
                true => (0.01, 0.04),
                false => (0.05, 0.1)
            };
            rand::thread_rng().gen_range(min, max)
        } 
    }
}

#[derive(PartialEq, Clone)]
pub enum ProjectileType {
    Laser,
    Asteroid(bool) // Flag for if this asteroid is a split
}

#[derive(Clone)]
struct Projectile {
    lifetime: f32,
    velocity: Vector2,
    rotation: f32,
    position: Point2,
    radius: f32,
    projectile_type: ProjectileType,
    collision_handle: usize
}

pub struct ProjectileShooter {
    projectiles: Vec<Projectile>,
    recycle_indices: VecDeque<usize>
}

impl ProjectileShooter {
    pub fn new() -> Self {
        Self {
            projectiles: Vec::with_capacity(10),
            recycle_indices: VecDeque::with_capacity(10)
        } 
    }

    pub fn fire(&mut self,
        position: Point2,
        velocity: Vector2,
        rotation: f32,
        projectile_type: ProjectileType,
        collision_system: &mut collision::CollisionSystem)
    {
        let radius = generate_projectile_radius(&projectile_type);

        let team = match projectile_type {
            ProjectileType::Laser => 0,
            ProjectileType::Asteroid(_) => 1
        };

        let lifetime = match projectile_type {
            ProjectileType::Laser => LASER_LIFETIME,
            ProjectileType::Asteroid(_) => ASTEROID_LIFETIME
        };

        // Assuming this allocation takes place on the stack
        let new_projectile = Projectile {
            lifetime,
            velocity,
            rotation,
            position,
            radius,
            projectile_type,
            collision_handle: collision_system.create_collider(position, radius, team)
        };

        let recycle_index = self.recycle_indices.pop_front();
        if recycle_index.is_some() {
            // Recycle array element position and return
            self.projectiles[recycle_index.unwrap()] = new_projectile;
            return;
        }

        self.projectiles.push(new_projectile);
    }

    pub fn count_alive(&self, projectile_type: ProjectileType) -> u32 {
        let mut count = 0;
        for projectile in &self.projectiles[..] {
            if projectile.projectile_type == projectile_type {
                count += 1;
            }
        }

        count
    }

    pub fn move_projectiles(&mut self,
        world_size: Vector2,
        collision_system: &mut collision::CollisionSystem,
        dt: f32)
    {
        let mut asteroid_splits = Vec::new();

        for i in 0..self.projectiles.len() {
            let projectile = &mut self.projectiles[i];

            // Only update projectiles with remaining lifetime
            if projectile.lifetime > 0.0 {
                let did_collide_last_frame = collision_system.check_collider(projectile.collision_handle);
                if did_collide_last_frame {
                    if projectile.projectile_type == ProjectileType::Asteroid(false) {
                        asteroid_splits.push(projectile.position);
                    }

                    // Kill projectile by setting it's lifetime to 0
                    projectile.lifetime = 0.0;
                }
                
                projectile.lifetime -= dt;
                if projectile.lifetime <= 0.0 {
                    self.recycle_indices.push_back(i);
                    collision_system.release_collider(projectile.collision_handle);
                    continue;
                }

                projectile.position += projectile.velocity * dt;
                super::wrap_position(&mut projectile.position, &world_size, projectile.radius);
                collision_system.update_collider(projectile.collision_handle, projectile.position, projectile.radius);
            }
        }

        for split_pos in asteroid_splits {
            let mut rng = rand::thread_rng();
            let mut rotation = rng.gen_range(0.0, consts::PI * 2.0);
            for _ in 0..4 {
                let velocity = Vector2::new(rotation.cos(), rotation.sin()) * rng.gen_range(0.1, 0.2);
                rotation += consts::PI * 0.5;
                self.fire(split_pos, velocity, rng.gen_range(0.0, consts::PI * 2.0), ProjectileType::Asteroid(true), collision_system);
            }
        }
    }

    pub fn create_draw_data(&mut self) -> (Vec<gfx::ProjectileDrawData>, Vec<gfx::AsteroidDrawData>) {
        let mut lasers = Vec::with_capacity(10);
        let mut asteroids = Vec::with_capacity(10);

        for projectile in &mut self.projectiles[..] {
            if projectile.lifetime <= 0.0 {
                continue;
            }

            match projectile.projectile_type {
                // Create asteroid draw data
                ProjectileType::Asteroid(_) => {
                    asteroids.push(gfx::AsteroidDrawData {
                        position: projectile.position,
                        rotation: projectile.rotation,
                        radius: projectile.radius
                    });
                },

                // Create projectile (laser) draw data
                ProjectileType::Laser => {
                    lasers.push(gfx::ProjectileDrawData {
                        position: projectile.position,
                        rotation: projectile.rotation
                    });
                }
            }
        }

        (lasers, asteroids)
    }
}
