use code_test_lib:: { raycast, prelude::* };
use code_test_lib as ct;

pub fn test_ray_against_circle(ray_start: Point2, ray_end: Point2, circle_point: Point2, circle_radius: f32) -> bool {
    let ray = ray_end - ray_start;

    let to_target = circle_point - ray_start;
    let projection_scalar = to_target.dot(&ray) / ray.dot(&ray);

    if projection_scalar <= 0.0 {
        return false;
    }

    let projection = ray_start + projection_scalar * ray;
    let target_to_projection = projection - circle_point;

    target_to_projection.dot(&target_to_projection).sqrt() <= circle_radius
}

#[derive(Copy, Clone)]
struct RaycastTarget {
    position: Point2,
    radius: f32,
    kind: raycast::RayHitKind
}

pub struct RaycastProcessor {
    targets: Vec<RaycastTarget>
}

impl RaycastProcessor {
    pub fn new() -> Self {
        Self {
            targets: Vec::with_capacity(10)
        } 
    }

    pub fn add_target(&mut self, position: Point2, radius: f32) {
        self.targets.push(RaycastTarget { position, radius, kind: raycast::RayHitKind::Ship });
    }

    pub fn clear_targets(&mut self) {
        self.targets.clear();
    }
}

impl ct::raycast::RaycastProcessor for RaycastProcessor {
    fn process_raycasts(&self, _rays: &[ct::raycast::Raycast], _hits: &mut [Option<ct::raycast::RayHit>]) {
        for (idx, raycast) in _rays.iter().enumerate() {
            let end = raycast.origin + raycast.direction * 1000.0;

            for target in &self.targets[..] {
                if test_ray_against_circle(raycast.origin, end, target.position, target.radius) {
                    let to_hit = target.position - raycast.origin;
                    let to_hit_mag = to_hit.dot(&to_hit).sqrt();
                    if to_hit_mag < raycast.t_min { continue; }

                    let ray_hit = ct::raycast::RayHit { kind: raycast::RayHitKind::Ship, t: to_hit_mag };
                    _hits[idx] = Some(ray_hit);
                    break;
                }
            }
        }
    }
}
