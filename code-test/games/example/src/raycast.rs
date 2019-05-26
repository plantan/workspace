use code_test_lib:: { raycast, prelude::*, player };
use code_test_lib as ct;

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
            let ray = end - raycast.origin;

            // TODO: Pick ray hit with shortest length
            
            for target in &self.targets[..] {
                let to_target = target.position - raycast.origin;
                let projection_scalar = to_target.dot(&ray) / ray.dot(&ray);

                if projection_scalar <= 0.0 {
                    continue;
                }

                let projection = raycast.origin.coords + projection_scalar * ray;
                let target_to_projection = projection - target.position.coords;

                if target_to_projection.dot(&target_to_projection).sqrt() <= target.radius {
                    // We have a hit!
                    let to_hit = Point2::from_coordinates(projection) - raycast.origin;
                    let ray_hit = ct::raycast::RayHit { kind: raycast::RayHitKind::Ship, t: to_hit.dot(&to_hit).sqrt() };
                    _hits[idx] = Some(ray_hit);
                    break;
                }
            }
        }
    }
}
