package com.embedded.audiocontroller.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Lazy;
import org.springframework.stereotype.Service;
import jakarta.annotation.PostConstruct;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

@Service
public class AudioService {
    private static final String TRACK_DIRECTORY = "src/main/resources/songs/";
    private Map<Integer, String> songPaths = new HashMap<>();
    private AudioFormat audioFormat;
    private AudioInputStream audioInputStream;
    int bytesPerSample;
    int shiftAmount;
    private boolean playing = true;

    @Autowired
    @Lazy
    private SerialCommunication serialCommunication;


    @PostConstruct
    public void loadAllTracks() {
        songPaths.put(0, TRACK_DIRECTORY + "undefined.wav");
        songPaths.put(1, TRACK_DIRECTORY + "six16inLA.wav");
        songPaths.put(2, TRACK_DIRECTORY + "need2.wav");

    }

    public void loadTrack(int trackID) throws IOException, UnsupportedAudioFileException {
        playing = false;
        String filepath = songPaths.get(trackID);
        if (filepath == null) {
            throw new IllegalArgumentException("Track not found: " + trackID);
        }

        try {
            if (audioInputStream != null) {
                audioInputStream.close();
            }
        }
        catch (IOException e) {
            throw new IOException("Error closing existing audio input stream: " + e.getMessage());
        }

        try {
            File wavFile = new File(filepath);
            audioInputStream = AudioSystem.getAudioInputStream(wavFile);
            audioFormat = audioInputStream.getFormat();

            bytesPerSample = audioFormat.getSampleSizeInBits() / 8;
            shiftAmount = audioFormat.getSampleSizeInBits() - 12;

            playing = true;
            int sampleRate = (int) audioFormat.getFrameRate();
            serialCommunication.sendSampleRate(sampleRate);
            playing = true;
        }
        catch (UnsupportedAudioFileException e) {
            throw new UnsupportedAudioFileException("Error creating audio input stream: " + e.getMessage());
        }
    }


    public byte[] getNextSample() throws Exception {
        if (!playing || audioInputStream == null) {
            return new byte[]{0x00, 0x08};
        }
        int frameSize = audioFormat.getFrameSize();
        byte[] data = new byte[frameSize];
        byte[] outData = new byte[2];

        // Read one frame
        if (audioInputStream.read(data) == -1) {
            return null;
        }
        // Skip two frames
        audioInputStream.skip(frameSize * 2);

        // Merge bytes into a single sample
        int sample = 0;
        for (int j = 0; j < bytesPerSample; j++) {
            sample |= (data[j] & 0xFF) << (8 * j);
        }

        // Scale down to 12 bits (-2048-2048) and offset (0-4096)
        sample = sample >> shiftAmount;
        if ((sample & 0x0800) > 0) {
            sample |= 0xFFFFF000;
        }
        sample += 2048;

        // Package
        outData[0] = (byte) (sample & 0xFF);
        outData[1] = (byte) ((sample >> 8) & 0xFF);
        return outData;
    }

    public void play() {
        playing = true;
    }

    public void pause() {
        playing = false;
    }

}