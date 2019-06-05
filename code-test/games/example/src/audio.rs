use code_test_lib as ct;
use ct::prelude::*;
use std::collections::VecDeque;

#[derive(Clone, Copy)]
pub enum AudioRequest {
    IntroMusic(bool),
    GameplayMusic(bool),
    Laser,
    Confirm
}

pub struct AudioRequester {
    requests: VecDeque<AudioRequest>
}

impl AudioRequester {
    pub fn new() -> Self {
        Self {
            requests: VecDeque::new()
        }
    }

    pub fn add(&mut self, request: AudioRequest) {
        self.requests.push_back(request);
    }
}

pub struct AudioPlayer {
    intro_music: audio::Source,
    confirm_sound: audio::Source,
    gameplay_music: audio::Source,

    laser_sound_pool: [audio::Source; 5],
    laser_sound_pool_index: usize
}

impl AudioPlayer {
    pub fn new(ctx: &mut Context) -> Self {
        let mut intro_music = audio::Source::new(ctx, "/solstice.wav").unwrap();
        intro_music.set_repeat(false);

        let mut confirm_sound = audio::Source::new(ctx, "/confirm.wav").unwrap();
        confirm_sound.set_repeat(false);

        let mut gameplay_music = audio::Source::new(ctx, "/gradius.wav").unwrap();
        gameplay_music.set_repeat(false);

        let mut laser_sound_pool: [audio::Source; 5] = [
            audio::Source::new(ctx, "/laser.wav").unwrap(),
            audio::Source::new(ctx, "/laser.wav").unwrap(),
            audio::Source::new(ctx, "/laser.wav").unwrap(),
            audio::Source::new(ctx, "/laser.wav").unwrap(),
            audio::Source::new(ctx, "/laser.wav").unwrap()
        ];

        for laser_sound in &mut laser_sound_pool[..] { laser_sound.set_repeat(false); };
        
        Self {
            intro_music,
            confirm_sound,
            gameplay_music,
            laser_sound_pool,
            laser_sound_pool_index: 0
        }
    }

    pub fn play(&mut self, mut requester: AudioRequester) {
        while requester.requests.len() > 0 {
            match requester.requests.pop_back().unwrap() {
                AudioRequest::IntroMusic(play) => {
                    if play {
                        self.intro_music.play().ok();
                    } else {
                        self.intro_music.stop();
                    }
                },

                AudioRequest::GameplayMusic(play) => {
                    if play {
                        self.gameplay_music.play().ok();
                    } else {
                        self.gameplay_music.stop();
                    }
                },

                AudioRequest::Laser => self.play_laser_sound(),
                AudioRequest::Confirm => { self.confirm_sound.play().ok(); }
            }
        }
    }

    pub fn play_laser_sound(&mut self) {
        self.laser_sound_pool[self.laser_sound_pool_index].play().ok();

        self.laser_sound_pool_index += 1;
        if self.laser_sound_pool_index >= self.laser_sound_pool.len() {
            self.laser_sound_pool_index = 0;
        }
    }
}