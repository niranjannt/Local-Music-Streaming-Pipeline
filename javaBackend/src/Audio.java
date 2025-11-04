import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import jssc.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

// Receiving an 'R' to the serial port requests data and causes Audio class to send back data for both channels
// 00 - Halting sending data
// 01 - Right channel data
// 10 - Left channel data
// 11 - Init info

public class Audio {
    private static final Logger log = LoggerFactory.getLogger(Audio.class);
    private String path;
    private AudioFormat audioFormat;
    private final AudioInputStream audioInputStream;
    private final SerialPort serialPort;
    int bytesPerSample;
    int shiftAmount;
    boolean channel;

    Audio(String path) throws IOException {
        File wavFile = new File(path);
        FileInputStream wavInputStream = new FileInputStream(wavFile);
        setAudioFormat(wavInputStream);
        audioInputStream = new AudioInputStream(wavInputStream, audioFormat, wavFile.length());
        //serialPort = new SerialPort("/dev/cu.usbserial-A106DAXQ");    // Mac
        serialPort = new SerialPort("/dev/cu.usbserial-A50285BI");
        //serialPort = new SerialPort("COM12");                                // Windows

        openPort();
        serialPort.addEventListener(new PortReader(), SerialPort.MASK_RXCHAR);
        send1Data();
        //sendSampleRate();
        //sendInitData();
    }

    private void send1Data() throws IOException {
        channel = false;
        byte[] data = new byte[bytesPerSample];
        byte[] outData = new byte[2];
        for (int i = 0; i < 1000; i++) {
            //read one sample
            int bytesRead = audioInputStream.read(data, 0, bytesPerSample);

            //turn sample into int form
            int sample = 0;
            for (int j = 0; j < bytesPerSample; j++) {
                sample |= (data[j] & 0xFF) << (8 * j);
            }

            //scale down to 14 bits and add header
            sample = (sample >> shiftAmount) & 0x3FFF;
            sample |= channel ? 0x8000 : 0x4000;
            channel = !channel;

            outData[0] = (byte) (sample & 0xFF);
            outData[1] = (byte) ((sample >> 8) & 0xFF);
            serialPort.writeBytes(outData);
        }
    }
    /*
        Sets the AudioFormat attribute with the data specified in the given WAV file's header
     */
    private void setAudioFormat(FileInputStream wavInputStream) throws IOException {
        byte[] headerArr = new byte[4];
        byte[] chunkArr = new byte[4];

        //RIFF STRING
        wavInputStream.read(headerArr, 0, 4);
        System.out.println(new String(headerArr));

        //FILE SIZE
        wavInputStream.read(chunkArr, 0, 4);
        int fileSize = (chunkArr[0]) + ((chunkArr[1] & 0xFF) << 8) + ((chunkArr[2] & 0xFF) << 16) + ((chunkArr[3] & 0xFF) << 24);
        System.out.println("File size: " + fileSize);

        //WAVE STRING
        wavInputStream.read(headerArr, 0, 4);
        System.out.println(new String(headerArr));

        //FMT STRING
        wavInputStream.read(headerArr, 0, 4);
        System.out.println(new String(headerArr));

        //SUBCHUNK1
        wavInputStream.read(chunkArr, 0, 4);
        int chunk2 = (chunkArr[0]) + ((chunkArr[1] & 0xFF) << 8) + ((chunkArr[2] & 0xFF) << 16) + ((chunkArr[3] & 0xFF) << 24);
        System.out.println("subChunk1: " + chunk2);

        byte[] formatArr = new byte[2];
        wavInputStream.read(formatArr, 0,2);
        int format = (formatArr[0]) + ((formatArr[1] & 0xFF) << 8);
        AudioFormat.Encoding encoding = null;
        switch (format) {
            //TODO CHECK FOR SIGNED/UNSIGNED
            case 0x01:
                System.out.println("AudioFormat: PCM");
                encoding = AudioFormat.Encoding.PCM_SIGNED;
                break;
            case 0x03:
                System.out.println("IEEE float");
                encoding = AudioFormat.Encoding.PCM_FLOAT;
                break;
            case 0x06:
                System.out.println("8-bit ITU-T G.711 A-law");
                encoding = AudioFormat.Encoding.ALAW;
                break;
            case 0x07:
                System.out.println("8-bit ITU-T G.711 µ-law");
                encoding = AudioFormat.Encoding.ULAW;
                break;
            default:
                System.out.println("Unhandled case: " + format);
        }

        byte[] numChannelsArr = new byte[2];
        wavInputStream.read(numChannelsArr, 0, 2);
        int numChannels = (numChannelsArr[0]) + (numChannelsArr[1]);
        switch (numChannels) {
            case 1:
                System.out.println("Mono");
            case 2:
                System.out.println("Stereo");
        }

        byte[] sampleRateArr = new byte[4];
        wavInputStream.read(sampleRateArr, 0, 4);
        int sampleRate = ((sampleRateArr[0] & 0xFF) | ((sampleRateArr[1] & 0xFF) << 8)| ((sampleRateArr[2] & 0xFF) << 16) | ((sampleRateArr[3] & 0xFF) << 24));
        System.out.println("Sample rate: " + sampleRate);

        byte[] byteRateArr = new byte[4];
        wavInputStream.read(byteRateArr, 0, 4);
        int byteRate = ((byteRateArr[0] & 0xFF) | ((byteRateArr[1] & 0xFF) << 8)| ((byteRateArr[2] & 0xFF) << 16) | ((byteRateArr[3] & 0xFF) << 24));
        System.out.println("Byte rate: " + byteRate);

        byte[] blockAlignArr = new byte[2];
        wavInputStream.read(blockAlignArr, 0, 2);
        int blockAlign = (blockAlignArr[0]) + (blockAlignArr[1]);
        System.out.println("Block align: " + blockAlign);

        byte[] BPSArr = new byte[2];
        wavInputStream.read(BPSArr, 0, 2);
        int bitsPerSample = (BPSArr[0]) + (BPSArr[1]);
        System.out.println("Bits per sample: " + bitsPerSample);

        //DATA STRING
        String dataString = "";
        while (!dataString.equals("data")) {
            wavInputStream.read(headerArr, 0, 4);
            dataString = new String(headerArr);
        }
        System.out.println(dataString);

        //for PCM frame rate is equal to sample rate
        this.audioFormat = new AudioFormat(encoding, sampleRate, bitsPerSample, numChannels, blockAlign, sampleRate, false);
    }

    /*
        Opens and sets parameters for the SerialPort
     */
    private void openPort() {
        try {
            this.serialPort.openPort();
            this.serialPort.setParams(
                    2_000_000,
                    SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1,
                    SerialPort.PARITY_NONE);
        } catch (SerialPortException e) {
            throw new RuntimeException(e);
        }
    }

    /*
        Sends sample data in three bytes:
        Header byte: 0xC0 (header of 11) tells ESP that we're about to send two bytes of data
        Byte 0: First byte of sample rate
        Byte 1: Second byte of sample rate
        Sample rate = (Byte1 << 8) | Byte0
     */
    private void sendSampleRate() {
        System.out.println("sending sample rate");
        try {
            int frameRate = (int) this.audioFormat.getFrameRate();
            byte header = (byte) 0xC0;
            byte byte0 = (byte) (frameRate & 0xFF);
            byte byte1 = (byte) ((frameRate >> 8) & 0xFF);
            serialPort.writeBytes(new byte[]{header, byte0, byte1});
        }
        catch (SerialPortException ex) {
            log.error("e: ", ex);
        }
    }

    /*
        Sends the first thousand samples
        Each sample is two bytes, lower 14 bits are data and top 2 bits are header as defined below
        01 - Right channel data
        10 - Left channel data
     */
    private void sendInitData() throws IOException {
        System.out.println("sending init data");
        channel = false;
        byte[] data = new byte[bytesPerSample];
        byte[] outData = new byte[2];
        for (int i = 0; i < 1000; i++) {
            //read one sample
            int bytesRead = audioInputStream.read(data, 0, bytesPerSample);

            //turn sample into int form
            int sample = 0;
            for (int j = 0; j < bytesPerSample; j++) {
                sample |= (data[j] & 0xFF) << (8 * j);
            }

            //scale down to 14 bits and add header
            sample = (sample >> shiftAmount) & 0x3FFF;
            sample |= channel ? 0x8000 : 0x4000;
            channel = !channel;

            outData[0] = (byte) (sample & 0xFF);
            outData[1] = (byte) ((sample >> 8) & 0xFF);
            serialPort.writeBytes(outData);
        }
    }

    /*
        PortReader class handles waiting to receive a request from the ESP and a single sample data in return
     */
    private class PortReader implements SerialPortEventListener {
        @Override
        public void serialEvent(SerialPortEvent serialPortEvent) {
            if (serialPortEvent.isRXCHAR() && serialPortEvent.getEventValue() > 0) {
                try {
                    byte[] recieved = serialPort.readBytes(1);
                    if (recieved[0] == 'R') {
                        sendSample();
                    }
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
        }

        private void sendSample() throws IOException {
            channel = false;
            byte[] data = new byte[bytesPerSample];
            byte[] outData = new byte[2];
            for (int i = 0; i < 1000; i++) {
                //read one sample
                int bytesRead = audioInputStream.read(data, 0, bytesPerSample);

                //turn sample into int form
                int sample = 0;
                for (int j = 0; j < bytesPerSample; j++) {
                    sample |= (data[j] & 0xFF) << (8 * j);
                }

                //scale down to 14 bits and add header
                sample = (sample >> shiftAmount) & 0x3FFF;
                sample |= channel ? 0x8000 : 0x4000;
                channel = !channel;

                outData[0] = (byte) (sample & 0xFF);
                outData[1] = (byte) ((sample >> 8) & 0xFF);
                serialPort.writeBytes(outData);
            }
        }
    }
}
