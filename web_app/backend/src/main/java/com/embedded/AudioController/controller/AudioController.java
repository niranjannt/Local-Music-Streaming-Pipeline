package com.embedded.audiocontroller.controller;

import com.embedded.audiocontroller.service.AudioService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
@RequestMapping("/api/audio")
public class AudioController {

    @Autowired
    private AudioService audioService;


    @PostMapping("/load/{trackId}")
    public ResponseEntity<?> loadTrack(@PathVariable int trackId) {
        try {
            audioService.loadTrack(trackId);
            return ResponseEntity.ok().build();
        }
        catch (Exception e) {
            return ResponseEntity.badRequest().body(Map.of("error", e.getMessage()));
        }
    }

    @PostMapping("/play")
    public ResponseEntity<?> play() {
        audioService.play();
        return ResponseEntity.ok().build();
    }

    @PostMapping("/pause")
    public ResponseEntity<?> pause() {
        audioService.pause();
        return ResponseEntity.ok().build();
    }
}