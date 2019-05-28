use code_test_lib::{ prelude::*, gfx };
use super::collision;
use std::collections::VecDeque;
use std::f32::consts;
use rand::Rng;

const LASER_LIFETIME: f32 = 3.0;
const ASTEROID_LIFETIME: f32 = 999.0;

fn get_projectile_radius(projectile_type: &ProjectileType) -> f32 {
    match projectile_type {
        ProjectileType::Asteroid => rand::thread_rng().gen_range(0.1, 0.2),
        ProjectileType::Laser => 0.1
    }
}

#[derive(PartialEq, Clone)]
pub enum ProjectileType {
    Laser,
    Asteroid
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
        mut rotation: f32,
        projectile_type: ProjectileType,
        collision_processor: &mut collision::CollisionSystem)
    {
        rotation += consts::PI * 0.5; // Hack for rotation!

        let radius = get_projectile_radius(&projectile_type);

        let team = match projectile_type {
            ProjectileType::Laser => 3,
            ProjectileType::Asteroid => 6
        };

        let lifetime = match projectile_type {
            ProjectileType::Laser => LASER_LIFETIME,
            ProjectileType::Asteroid => ASTEROID_LIFETIME
        };

        let new_projectile = Projectile {
            lifetime,
            velocity,
            rotation,
            position,
            radius,
            projectile_type,
            collision_handle: collision_processor.create_collider(position, radius, team)
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

    pub fn update(&mut self,
        screen_size: (u32, u32),
        world_to_screen: graphics::Matrix4,
        collision_system: &mut collision::CollisionSystem,
        dt: f32)
    {
        for i in 0..self.projectiles.len() {
            let projectile = &mut self.projectiles[i];

            // Only update projectiles with remaining lifetime
            if projectile.lifetime > 0.0 {
                let did_collide_last_frame = collision_system.check_collider(projectile.collision_handle);
                if did_collide_last_frame {
                    // Kill projectile by setting it's lifetime to 0
                    projectile.lifetime = 0.0;
                }
                
                projectile.lifetime -= dt;
                if projectile.lifetime <= 0.0 {
                    self.recycle_indices.push_back(i);
                    continue;
                }

                projectile.position += projectile.velocity * dt;

                // Wrap world position
                let screen_pos = world_to_screen * na::Vector4::new(projectile.position.x, projectile.position.y, 0.0, 1.0);
                if screen_pos.x > screen_size.0 as f32 || screen_pos.x < 0.0 {
                    projectile.position.x = -projectile.position.x;
                }
                if screen_pos.y > screen_size.1 as f32 || screen_pos.y < 0.0 {
                    projectile.position.y = -projectile.position.y;
                }

                collision_system.update_collider(projectile.collision_handle, projectile.position, projectile.radius);
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
                ProjectileType::Asteroid => {
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
