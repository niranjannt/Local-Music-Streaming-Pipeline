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
    boolean send;

    Audio(String path) throws IOException, UnsupportedAudioFileException, InterruptedException {
        File wavFile = new File(path);

        audioInputStream = AudioSystem.getAudioInputStream(wavFile);
        audioFormat = audioInputStream.getFormat();

        bytesPerSample = audioFormat.getSampleSizeInBits() / 8;
        shiftAmount = audioFormat.getSampleSizeInBits() - 12;

        System.out.println("Format: " + audioFormat);
        System.out.println("Channels: " + audioFormat.getChannels());
        System.out.println("Sample rate: " + audioFormat.getSampleRate());
        System.out.println("Bytes per sample: " + bytesPerSample);

        serialPort = new SerialPort("/dev/cu.usbserial-A106DAXQ");         // Mac usbB
        //serialPort = new SerialPort("/dev/cu.usbserial-A50285BI"); // Macro usb calculator one
        //serialPort = new SerialPort("COM7");


        openPort();
        serialPort.addEventListener(new PortReader(), SerialPort.MASK_RXCHAR);
        //readAllFrames();
        //sendSample();

        sendSampleRate();
//        sendInitData();
//        Thread.sleep(1000);
//        sendAllSamples();
        //sendInitData();

    }

    /*
        Opens and sets parameters for the SerialPort
     */
    private void openPort() {
        try {
            this.serialPort.openPort();
            this.serialPort.setParams(
                    3_000_000,
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
        send = true;
        for (int i = 0; i < 2048; i++) {
            sendSample();
            send = !send;
        }
    }

    /*
        PortReader class handles waiting to receive a request from the ESP and a single sample data in return
     */
    private class PortReader implements SerialPortEventListener {
        @Override
        public void serialEvent(SerialPortEvent serialPortEvent) {
           // System.out.println("received event");
            if (serialPortEvent.isRXCHAR() && serialPortEvent.getEventValue() > 0) {
                try {
                    byte[] recieved = serialPort.readBytes(1);
                    if (recieved[0] == 'B') {
                        sendBurst();
                    }
                    if (recieved[0] == 'R') {
                        sendSample();
                    }
                    if (recieved[0] == 'I') {
                        sendInitData();
                    }
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
        }
    }

    /*
        This function is called whenever an 'R' is received, it sends one
        frame (a sample for each channel) to the ESP
    */
    private boolean sendSample1() throws IOException {
        int frameSize = audioFormat.getFrameSize();
        byte[] data = new byte[frameSize];
        byte[] outData = new byte[2];

        //read one frame
        if (audioInputStream.read(data) == -1) {
            return false;
        }

        //turn samples into int form
        int rightSample = 0;
        int leftSample = 0;
        for (int j = 0, k = bytesPerSample; k < frameSize; j++, k++) {
            rightSample |= (data[j] & 0xFF) << (8 * j);
            leftSample  |= (data[k] & 0xFF) << (8 * j);
        }
        leftSample = leftSample >> shiftAmount;


        // Scale down to 14 bits
//        leftSample  = (leftSample >> shiftAmount) & 0x3FFF;
//        rightSample = (rightSample >> shiftAmount) & 0x3FFF;
//
//        // Add header
//        leftSample  |= 0x8000;
//        rightSample |= 0x4000;
//        System.out.println(Integer.toHexString(leftSample));
//        System.out.println("Right Sample raw: 0x" + Integer.toHexString(rightSample));

        // Package
        outData[0] = (byte) (leftSample & 0xFF);
        outData[1] = (byte) ((leftSample >> 8) & 0xFF);
        //outData[2] = (byte) ((rightSample & 0xFF));
        //outData[3] = (byte) ((rightSample >> 8) & 0xFF);
        serialPort.writeBytes(outData);
        return true;
    }

    private boolean sendSample() throws IOException {
        int frameSize = audioFormat.getFrameSize();
        byte[] data = new byte[frameSize];
        byte[] outData = new byte[2];

        //read one frame
        if (audioInputStream.read(data) == -1) {
            return false;
        }

        if (!send) {
            return true;
        }
        //turn samples into int form (treating as SIGNED)
        int leftSample = 0;
        for (int j = 0; j < bytesPerSample; j++) {
            leftSample |= (data[j] & 0xFF) << (8 * j);
        }

        // Scale down to 12 bits (for your DAC which uses 12 bits: 0x0fff mask)
        // Shift to 12-bit unsigned (0 to 4095)
        leftSample = leftSample >> shiftAmount;
        if ((leftSample & 0x0800) > 0) {
            leftSample |= 0xFFFFF000;
        }
        leftSample += 2048; // Convert to unsigned 12-bit centered at 2048


        // Package
        outData[0] = (byte) (leftSample & 0xFF);
        outData[1] = (byte) ((leftSample >> 8) & 0xFF);
        serialPort.writeBytes(outData);
        return true;
    }

    private void sendBurst() throws IOException {
        send = true;
        for (int i = 0; i < 10000; i++) {
            sendSample();
            send = !send;
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
