package com.embedded.audiocontroller.service;

import jssc.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.context.annotation.Lazy;
import jakarta.annotation.PostConstruct;
import jakarta.annotation.PreDestroy;


@Service
public class SerialCommunication {
    private SerialPort serialPort;
    private volatile boolean running = true;

    @Autowired
    @Lazy
    private AudioService audioService;

    @PostConstruct
    public void initialize() {
        try {
            serialPort = new SerialPort("/dev/cu.usbserial-A106DAXQ");
            openPort();
            serialPort.addEventListener(new PortReader(), SerialPort.MASK_RXCHAR);
        }
        catch (SerialPortException e) {
            throw new RuntimeException(e);
        }
    }

    private void openPort() {
        try {
            this.serialPort.openPort();
            this.serialPort.setParams(
                    300_000,
                    SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1,
                    SerialPort.PARITY_NONE);
        }
        catch (SerialPortException e) {
            throw new RuntimeException(e);
        }
    }

    private class PortReader implements SerialPortEventListener {
        @Override
        public void serialEvent(SerialPortEvent serialPortEvent) {
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
                        sendBurst();
                    }
                }
                catch (Exception e) {
                    throw new RuntimeException(e);
                }
            }
        }
    }

    public void sendSampleRate(int sampleRate) {
        try {
            byte header = (byte) 0xC0;
            byte verify = (byte) 0xC0;
            byte byte0 = (byte) (sampleRate & 0xFF);
            byte byte1 = (byte) ((sampleRate >> 8) & 0xFF);
            serialPort.writeBytes(new byte[]{header, verify, byte0, byte1});
        }
        catch (SerialPortException e) {
            throw new RuntimeException(e);
        }
    }

    private void sendSample() throws Exception {
        byte outData[] = audioService.getNextSample();
        if (outData != null) {
            try {
                serialPort.writeBytes(outData);
            }
            catch (SerialPortException e) {
                throw new RuntimeException(e);
            }
        }
    }

    private void sendBurst() throws Exception {
        for (int i = 0; i < 2048; i++) {
            sendSample();
        }
    }

    @PreDestroy
    public void cleanup() {
        try {
            running = false;
            serialPort.closePort();
        }
        catch (SerialPortException e) {
            throw new RuntimeException(e);
        }
    }
}

