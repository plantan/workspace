extern crate rodio;
use std::fs::File;
use std::io::BufReader;
use rodio::Source;

struct AudioPlayer {
    music: rodio::Decoder<std::io::BufReader<std::fs::File>>,
    player_laser: rodio::Decoder<std::io::BufReader<std::fs::File>>,
    enemy_laser: rodio::Decoder<std::io::BufReader<std::fs::File>>,
    asteroid_hit: rodio::Decoder<std::io::BufReader<std::fs::File>>
}

impl AudioPlayer {
    // pub fn new() -> Self {
    //     let file = File::open("lib/assets/gradius.mp3").unwrap();
    //     let music = rodio::Decoder::new(BufReader::new(file)).unwrap();

    //     let file = File::open("lib/assets/batman_music.mp3").unwrap();
    //     let player_laser = rodio::Decoder::new(BufReader::new(file)).unwrap();

    //     let file = File::open("lib/assets/batman_music.mp3").unwrap();
    //     let enemy_laser = rodio::Decoder::new(BufReader::new(file)).unwrap();

    //     let file = File::open("lib/assets/batman_music.mp3").unwrap();
    //     let asteroid_hit = rodio::Decoder::new(BufReader::new(file)).unwrap();

    //     Self {
    //         music: 
    //         player_laser:
    //         enemy_laser:
    //         asteroid_hit:
    //     }
    // }
}

pub fn play_music() {
    // let device = rodio::default_output_device().unwrap();
    // let file = File::open("lib/assets/gradius.wav").unwrap();
    // let source = rodio::Decoder::new(BufReader::new(file)).unwrap();
    // rodio::play_raw(&device, source.convert_samples());
}