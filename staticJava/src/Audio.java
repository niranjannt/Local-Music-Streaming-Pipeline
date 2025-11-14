import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;
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

    Audio(String path) throws IOException, UnsupportedAudioFileException {
        File wavFile = new File(path);

        audioInputStream = AudioSystem.getAudioInputStream(wavFile);
        audioFormat = audioInputStream.getFormat();

        bytesPerSample = audioFormat.getSampleSizeInBits() / 8;
        shiftAmount = audioFormat.getSampleSizeInBits() - 14;

        System.out.println("Format: " + audioFormat);
        System.out.println("Channels: " + audioFormat.getChannels());
        System.out.println("Sample rate: " + audioFormat.getSampleRate());
        System.out.println("Bytes per sample: " + bytesPerSample);

        //serialPort = new SerialPort("/dev/cu.usbserial-A106DAXQ");         // Mac usbB
        serialPort = new SerialPort("/dev/cu.usbserial-A50285BI"); // Macro usb calculator one
        //serialPort = new SerialPort("COM12");                             // Windows

        openPort();
        sendSampleRate();
        sendInitData();

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
            byte header0 = (byte) 0x00;
            byte header1 = (byte) 0xC0;
            byte byte0 = (byte) (frameRate & 0xFF);
            byte byte1 = (byte) ((frameRate >> 8) & 0xFF);
            serialPort.writeBytes(new byte[]{header0, header1, byte0, byte1});
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
        int frameSize = audioFormat.getFrameSize();
        byte[] data = new byte[frameSize];
        byte[] outData = new byte[4]; //two 16 bit words - 4 bytes
        for (int i = 0; i < 1000; i++) {
            //read one sample
            int bytesRead = audioInputStream.read(data);

            //turn sample into int form
            int rightSample = 0;
            int leftSample = 0;
            for (int j = 0, k = bytesPerSample; j < frameSize; j++, k++) {
                rightSample |= (data[j] & 0xFF) << (8 * j);
                leftSample  |= (data[k] & 0xFF) << (8 * k);
            }


            //scale down to 14 bits and add header
            leftSample = (leftSample >> shiftAmount) & 0x3FFF;
            rightSample = (rightSample >> shiftAmount) & 0x3FFF;

            leftSample  |= 0x8000;
            rightSample |= 0x4000;


            outData[0] = (byte) (leftSample & 0xFF);
            outData[1] = (byte) ((leftSample >> 8) & 0xFF);
            outData[2] = (byte) ((rightSample & 0xFF));
            outData[3] = (byte) ((rightSample >> 8) & 0xFF);
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

        /*
            This function is called whenever an 'R' is received, it sends one
            frame (a sample for each channel) to the ESP
         */
        private void sendSample() throws IOException {
            int frameSize = audioFormat.getFrameSize();
            byte[] data = new byte[frameSize];
            byte[] outData = new byte[4];

            //read one frame
            int bytesRead = audioInputStream.read(data);

            //turn samples into int form
            int rightSample = 0;
            int leftSample = 0;
            for (int j = 0, k = bytesPerSample; k < frameSize; j++, k++) {
                rightSample |= (data[j] & 0xFF) << (8 * j);
                leftSample  |= (data[k] & 0xFF) << (8 * j);
            }


            // Scale down to 14 bits
            leftSample  = (leftSample >> shiftAmount) & 0x3FFF;
            rightSample = (rightSample >> shiftAmount) & 0x3FFF;

            // Add header
            leftSample  |= 0x8000;
            rightSample |= 0x4000;

            // Package
            outData[0] = (byte) (leftSample & 0xFF);
            outData[1] = (byte) ((leftSample >> 8) & 0xFF);
            outData[2] = (byte) ((rightSample & 0xFF));
            outData[3] = (byte) ((rightSample >> 8) & 0xFF);
            serialPort.writeBytes(outData);
        }
    }

    /*
        Test function that reads one frame of data
        Same as the function sendSample but doesn't require port and can be called without event listener
     */
    private void printOneFrame() throws IOException {
        int frameSize = audioFormat.getFrameSize();
        byte[] data = new byte[frameSize];
        byte[] outData = new byte[4];

        //read one frame
        int bytesRead = audioInputStream.read(data);

        //turn samples into int form
        int rightSample = 0;
        int leftSample = 0;
        for (int j = 0, k = bytesPerSample; k < frameSize; j++, k++) {
            rightSample |= (data[j] & 0xFF) << (8 * j);;
            leftSample  |= (data[k] & 0xFF) << (8 * j);
        }

        System.out.println("left Sample raw: 0x" + Integer.toHexString(leftSample));
        System.out.println("Right Sample raw: 0x" + Integer.toHexString(rightSample));


        // Scale down to 14 bits
        leftSample = (leftSample >> shiftAmount) & 0x3FFF;
        rightSample = (rightSample >> shiftAmount) & 0x3FFF;

        // Add header
        leftSample |= 0x8000;
        rightSample |= 0x4000;

        System.out.println("left Sample final: 0x" + Integer.toHexString(leftSample));
        System.out.println("Right Sample final: 0x" + Integer.toHexString(rightSample));
    }

    /*
        Test function that goes through all data
        Doesn't need an open port
     */
    private void readAllFrames() throws IOException {
        int frameSize = audioFormat.getFrameSize();
        byte[] data = new byte[frameSize];
        byte[] outData = new byte[4];

        //read one frame
        while (audioInputStream.read(data) != -1) {

            //turn samples into int form
            int rightSample = 0;
            int leftSample = 0;
            for (int j = 0, k = bytesPerSample; k < frameSize; j++, k++) {
                rightSample |= (data[j] & 0xFF) << (8 * j);
                leftSample |= (data[k] & 0xFF) << (8 * j);
            }

            System.out.println("left Sample raw: 0x" + Integer.toHexString(leftSample));
            System.out.println("Right Sample raw: 0x" + Integer.toHexString(rightSample));


            // Scale down to 14 bits
            leftSample = (leftSample >> shiftAmount) & 0x3FFF;
            rightSample = (rightSample >> shiftAmount) & 0x3FFF;

            // Add header
            leftSample |= 0x8000;
            rightSample |= 0x4000;

            System.out.println("left Sample final: 0x" + Integer.toHexString(leftSample));
            System.out.println("Right Sample final: 0x" + Integer.toHexString(rightSample));
        }
    }
}
