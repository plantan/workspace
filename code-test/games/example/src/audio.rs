use code_test_lib as ct;
use ct::prelude::*;
use std::collections::VecDeque;

#[derive(Clone, Copy)]
pub enum AudioRequest {
    IntroMusic(bool),
    GameplayMusic(bool),
    GameOverMusic(bool),
    Laser,
    Confirm,
    Death,
    Hit
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
    music: Option<audio::Source>,
    confirm_sound: audio::Source,
    death_sound: audio::Source,

    hit_sound_pool: Vec<audio::Source>,
    hit_sound_pool_index: usize,

    laser_sound_pool: Vec<audio::Source>,
    laser_sound_pool_index: usize
}

impl AudioPlayer {
    pub fn new(ctx: &mut Context) -> Self {
        let mut confirm_sound = audio::Source::new(ctx, "/confirm.wav").unwrap();
        let mut death_sound = audio::Source::new(ctx, "/death.wav").unwrap();

        confirm_sound.set_repeat(false);
        death_sound.set_repeat(false);

        let laser_sound_pool = AudioPlayer::create_audio_pool(ctx, "/laser.wav", 5);
        let hit_sound_pool = AudioPlayer::create_audio_pool(ctx, "/hit.wav", 5);
        
        Self {
            music: None,
            confirm_sound,
            death_sound,

            hit_sound_pool,
            hit_sound_pool_index: 0,

            laser_sound_pool,
            laser_sound_pool_index: 0
        }
    }

    fn create_audio_pool(ctx: &mut Context, path: &str, size: usize) -> Vec<audio::Source> {
        let mut pool = Vec::new();
        for _ in 0..size {
            let mut sound = audio::Source::new(ctx, path).unwrap();
            sound.set_repeat(false);
            pool.push(sound);
        }

        pool
    }

    // Creating new sound sorce for music every time we play it, since calling stop()
    // on a Audio Source seems to make it unplayable after. Could this be a bug?
    fn load_and_play_music(ctx: &mut Context, path: &str) -> audio::Source {
        let music = audio::Source::new(ctx, path).unwrap();
        music.play().ok();
        music
    }

    fn try_stop_music(&mut self) {
        match &self.music {
            Some(s) => s.stop(),
            None => ()
        }
    }

    pub fn play(&mut self, ctx: &mut Context, mut requester: AudioRequester) {
        while requester.requests.len() > 0 {
            match requester.requests.pop_front().unwrap() {
                AudioRequest::IntroMusic(play) => {
                    if play {
                        self.music = Some(AudioPlayer::load_and_play_music(ctx, "/solstice.wav"));
                    } else {
                        self.try_stop_music();
                    }
                },

                AudioRequest::GameplayMusic(play) => {
                    if play {
                        self.music = Some(AudioPlayer::load_and_play_music(ctx, "/gradius.wav"));
                    } else {
                        self.try_stop_music();
                    }
                },

                AudioRequest::GameOverMusic(play) => {
                    if play {
                        self.music = Some(AudioPlayer::load_and_play_music(ctx, "/goemon.wav"));
                    } else {
                        self.try_stop_music();
                    }
                },

                AudioRequest::Laser => self.play_laser_sound(),
                AudioRequest::Hit => self.play_hit_sound(),

                AudioRequest::Confirm => { self.confirm_sound.play().ok(); },
                AudioRequest::Death => { self.death_sound.play().ok(); },
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

    pub fn play_hit_sound(&mut self) {
        self.hit_sound_pool[self.hit_sound_pool_index].play().ok();

        self.hit_sound_pool_index += 1;
        if self.hit_sound_pool_index >= self.hit_sound_pool.len() {
            self.hit_sound_pool_index = 0;
        }
    }
}