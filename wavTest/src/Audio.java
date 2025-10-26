import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import jssc.SerialPort;
import jssc.SerialPortException;

// Receiving an 'R' to the serial port requests data and causes Audio class to send back data for both channels
// 00 - Halting sending data
// 01 - Right channel info
// 10 - Left channel info
// 11 - Init info

public class Audio {
    private String path;
    private AudioFormat audioFormat;
    private AudioInputStream audioInputStream;
    private SerialPort serialPort;

    Audio(String path) throws IOException {
        File wavFile = new File(path);
        FileInputStream wavInputStream = new FileInputStream(wavFile);
        setAudioFormat(wavInputStream);
        audioInputStream = new AudioInputStream(wavInputStream, audioFormat, wavFile.length());
        serialPort = new SerialPort("COM12");       //mac dependent if running on windows find your port
        sendInitData();
    }

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
                System.out.println("Unhandled case");
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

        //TODO FIX BIT MANIPULATION FOR ALL
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

    private void sendInitData() {
        System.out.println("sending sample rate");
        try {
            this.serialPort.openPort();
            this.serialPort.setParams(
                    SerialPort.BAUDRATE_9600,
                    SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1,
                    SerialPort.PARITY_NONE);

            int frameRate = (int) this.audioFormat.getFrameRate();
            byte header = (byte) 0xC0;
            byte byte0 = (byte) (frameRate & 0xFF);
            byte byte1 = (byte) ((frameRate >> 8) & 0xFF);
            byte[] sampleRateArr = {byte0, byte1};

            serialPort.writeBytes(new byte[]{header, byte1, byte0});

            int bytesPerSample = audioFormat.getSampleSizeInBits() / 8;
            int shiftAmount = audioFormat.getSampleSizeInBits() - 14;
            boolean channel = false;
            for (int i = 0; i < 1000; i++) {
                //read one sample
                byte[] data = serialPort.readBytes(bytesPerSample);

                //turn sample into int form
                int sample = 0;
                for (int j = 0; j < bytesPerSample; j++) {
                    sample |= (data[j] & 0xFF) << (8 * j);
                }

                //scale down to 14 bits and add header
                sample >>= shiftAmount;
                sample |= channel ? 0x80 : 0x40;
                channel = !channel;

                for (int k = 0; k < bytesPerSample; k++) {
                    data[k] = (byte) ((sample >> (8 * k)) & 0xFF);
                }
                //add header in top two bits. 10 - left channel, 01 - right channel
                serialPort.writeBytes(data);
            }
        }
        catch (SerialPortException ex) {
            System.out.println(ex);
        }
    }
}
