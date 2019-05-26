use code_test_lib::prelude::*;
use std::collections::VecDeque;
use super::ct::gfx;
use std::f32::consts;

// TODO: Change from Projectile to some other more general name 
// that works for both projectiles and asteroids!

const PROJECTILE_LIFETIME: f32 = 3.0;
const PROJECTILE_COOLDOWN: f32 = 0.2;

pub enum ProjectileType {
    Projectile,
    Asteroid
}

struct Projectile {
    lifetime: f32,
    direction: Vector2,
    rotation: f32,
    position: Point2,
    projectile_type: ProjectileType
}

pub struct ProjectileShooter {
    projectiles: Vec<Projectile>,
    recycle_indices: VecDeque<usize>,
    cooldown: f32
}

impl ProjectileShooter {
    pub fn new() -> Self {
        Self {
            projectiles: Vec::with_capacity(10),
            recycle_indices: VecDeque::with_capacity(10),
            cooldown: 0.0
        } 
    }

    pub fn fire(&mut self, position: Point2, mut rotation: f32, projectile_type: ProjectileType) {
        if self.cooldown > 0.0 { return; }
        self.cooldown = PROJECTILE_COOLDOWN;

        let direction = Vector2::new(rotation.cos(), rotation.sin()).normalize();
        rotation += consts::PI * 0.5; // Hack for rotation!

        let recycle_index = self.recycle_indices.pop_front();
        if recycle_index.is_some() {
            let projectile = &mut self.projectiles[recycle_index.unwrap()];
            projectile.position = position;
            projectile.direction = direction;
            projectile.rotation = rotation;
            projectile.lifetime = PROJECTILE_LIFETIME;
            projectile.projectile_type = projectile_type;

            // Projectile recycled, just return
            return;
        }

        self.projectiles.push(Projectile {
            lifetime: PROJECTILE_LIFETIME,
            direction,
            rotation,
            position: position,
            projectile_type,
        });
    }

    pub fn update(&mut self, dt: f32) {
        self.cooldown -= dt;

        for i in 0..self.projectiles.len() {
            let projectile = &mut self.projectiles[i];

            if projectile.lifetime > 0.0 {
                projectile.position += projectile.direction * dt * 3.5;

                projectile.lifetime -= dt;
                if projectile.lifetime <= 0.0 {
                    self.recycle_indices.push_back(i);
                }
            }
        }
    }

    pub fn create_draw_data(&mut self) -> (Vec<gfx::ProjectileDrawData>, Vec<gfx::AsteroidDrawData>) {
        let mut projectiles = Vec::with_capacity(10);
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
                        radius: 1.0
                    });
                },

                // Create projectile draw data
                ProjectileType::Projectile => {
                    projectiles.push(gfx::ProjectileDrawData {
                        position: projectile.position,
                        rotation: projectile.rotation
                    });
                }
            }
        }

        (projectiles, asteroids)
    }
}
