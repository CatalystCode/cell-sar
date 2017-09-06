package com.microsoft.cellsar.ocp;

import android.speech.RecognizerIntent;

/**
 * Created by t-catayl on 9/6/2017.
 */

public class DttMessage {

    public static final int DTT_HEADER_LENGTH = 4;
    public static final int DTT_MAX_PACKET_SIZE = 100;
    public static final int DTT_MAX_PACKET_DATA_SIZE = 100 - 4;

    public static final int DTT_PACKET_ID_FIELD = 0;
    public static final int DTT_PACKET_COUNT_FIELD = 1;
    public static final int DTT_PACKET_TOTAL_FIELD = 2;
    public static final int DTT_PACKET_LENGTH_FIELD = 3;
    public static final int DTT_PACKET_DATA_FIELD = 4;

    private static int messageId = 0;
    public static final int MAX_MESSAGE_ID = 256;


    public enum DttMessageMode { SEND, RECEIVE }

    public final DttMessageMode mode;

    public int id;
    public int totalPackets;
    public String error;

    /* SEND mode only */
    private byte[] buffer;
    private int cursor;
    private int count;

    /* RECEIVE mode only */
    private byte[][] packets;

    public DttMessage(byte[] buffer) {
        mode = DttMessageMode.SEND;
        id = messageId;
        messageId = (messageId + 1) % MAX_MESSAGE_ID;
        this.buffer = buffer;
        cursor = 0;
        count = 0;
        totalPackets = buffer.length / DTT_MAX_PACKET_DATA_SIZE + 1;
    }

    public DttMessage() {
        mode = DttMessageMode.RECEIVE;
        packets = null;
    }

    public boolean hasNextPacket() {
        assert mode == DttMessageMode.SEND;
        return cursor < buffer.length;
    }

    public byte[] nextPacket() {
        assert mode == DttMessageMode.SEND;
        if (!hasNextPacket())
            return null;

        int datalen = count < totalPackets - 1
                ? DTT_MAX_PACKET_DATA_SIZE : buffer.length - cursor;
        int packet_size = DTT_HEADER_LENGTH + datalen;
        byte[] packet = new byte[packet_size];

        packet[DTT_PACKET_ID_FIELD] = (byte)id;
        packet[DTT_PACKET_COUNT_FIELD] = (byte)count;
        packet[DTT_PACKET_TOTAL_FIELD] = (byte)totalPackets;
        packet[DTT_PACKET_LENGTH_FIELD] = (byte)datalen;

        System.arraycopy(buffer, cursor, packet, DTT_PACKET_DATA_FIELD, datalen);

        count += 1;
        cursor += datalen;

        return packet;
    }

    public boolean isComplete() {
        assert mode == DttMessageMode.RECEIVE;
        if (packets == null)
            return false;

        for (int i = 0; i < packets.length; ++i) {
            if (packets[i] == null)
                return false;
        }

        return true;
    }

    public boolean addPacket(byte[] packet) {
        assert mode == DttMessageMode.RECEIVE;
        assert packet.length > DTT_HEADER_LENGTH;

        int packetId = packet[DTT_PACKET_ID_FIELD];
        int packetCount = packet[DTT_PACKET_COUNT_FIELD];
        int packetTotal = packet[DTT_PACKET_TOTAL_FIELD];
        int packetLength = packet[DTT_PACKET_LENGTH_FIELD];

        if (packets == null) {
            this.id = packetId;
            this.totalPackets = packetTotal;
            this.packets = new byte[packetTotal][];
        } else {
            if (this.id != packetId) {
                error = "bad packet id";
                return false;
            }
            if (this.totalPackets != packetTotal) {
                error = "bad packet total";
                return false;
            }
            if (packets[packetCount] != null) {
                error = "bad packet count";
                return false;
            }
        }

        packets[packetCount] = new byte[packetLength];
        System.arraycopy(packet, DTT_PACKET_DATA_FIELD, packets[packetCount], 0, packetLength);

        return true;
    }

    public byte[] getWholeMessage() {
        assert mode == DttMessageMode.RECEIVE;
        if (packets == null || !isComplete())
            return null;

        int total_length = 0;
        for (int i = 0; i < packets.length; ++i) {
            total_length += packets[i].length;
        }

        byte[] result = new byte[total_length];
        int cursor = 0;
        for (int i = 0; i < packets.length; ++i) {
            System.arraycopy(packets[i], 0, result, cursor, packets[i].length);
            cursor += packets[i].length;
        }

        return result;
    }
}
