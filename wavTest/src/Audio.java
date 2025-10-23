import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import jssc.SerialPort;
import jssc.SerialNativeInterface;
import jssc.SerialPortException;
import jssc.SerialPortList;

public class Audio {
    private String path;
    private AudioFormat audioFormat;
    private AudioInputStream audioInputStream;
    private SerialPort port;

    Audio(String path) throws IOException {
        File wavFile = new File(path);
        FileInputStream wavInputStream = new FileInputStream(wavFile);
        setAudioFormat(wavInputStream);
        this.audioInputStream = new AudioInputStream(wavInputStream, audioFormat, wavFile.length());
        this.port = new SerialPort("/dev/cu.usbserial-A106DAXQ");       //mac dependent if running on windows find your port
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
            this.port.openPort();
            this.port.setParams(
                    SerialPort.BAUDRATE_9600,
                    SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1,
                    SerialPort.PARITY_NONE);

            int frameRate = (int) this.audioFormat.getFrameRate();
            byte byte0 = (byte) (frameRate & 0xFF);
            byte byte1 = (byte) ((frameRate >> 8) & 0xFF);
            byte[] sampleRateArr = {byte0, byte1};

            this.port.writeBytes(new byte[]{byte1, byte0});//Write data to port
            this.port.closePort();//Close serial port
        }
        catch (SerialPortException ex) {
            System.out.println(ex);
        }
    }
}
