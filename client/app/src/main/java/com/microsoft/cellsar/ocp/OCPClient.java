package com.microsoft.cellsar.ocp;

import com.microsoft.cellsar.SARSMSConversationActivity;
import com.microsoft.cellsar.SMSMessageArrayListAdapter;

import dji.sdk.flightcontroller.FlightController;

/**
 * Created by t-catayl on 8/21/2017.
 */

public class OCPClient {

    private FlightController flightController;
    public SMSMessageArrayListAdapter chatAdapter;

    private static OCPClient instance;
    private OCPClient() {}
    public static OCPClient getInstance() {
        if (instance == null)
            instance = new OCPClient();
        return instance;
    }

    public void setFlightController(FlightController fc) {
        this.flightController = fc;
    }

    public void setPLMN(String mcc, String mnc) {
        sendData(buildPLMNPacket(mcc, mnc));
    }

    public void sendSMS(String imsi, String message) {
        if (message == null) message = "";
        if (message.length() > 75)
            message = message.substring(0, 75);
        sendData(buildSMSPacket(imsi, message));
    }

    private void sendData(byte[] data) {
        if (this.flightController == null)
            return;

        this.flightController.sendDataToOnboardSDKDevice(data, null);
    }

    private byte[] buildPLMNPacket(String mcc, String mnc) {
        byte[] data = new byte[100];

        /* PLMN packet
         *      [ 0..9 data type | 10..12 MCC | 13..15 MNC ]
         */

        writeToByteArray(data, 0, "plmn");

        assert mcc.length() == 3 : "mcc must be length 3";
        writeToByteArray(data, 10, mcc);

        assert mnc.length() == 3 : "mnc must be length 3";
        writeToByteArray(data, 13, mnc);

        return data;
    }

    private byte[] buildSMSPacket(String imsi, String message) {

        if (imsi == null) imsi = "111222333444555"; // temporary for debugging

        byte[] data = new byte[100];

        /* SMS packet
         *     [ 0..9 data type | 10..24 IMSI | 25..99 ]
         */

        writeToByteArray(data, 0, "sms");

        assert imsi.length() == 15 : "imsi must be length 15";
        writeToByteArray(data, 10, imsi);

        assert message.length() <= 75 : "message can't be longer than 75 characters";
        writeToByteArray(data, 25, message);

        return data;
    }

    private void writeToByteArray(byte[] byteArr, Integer start, String str) {
        if (start < 0 || start + str.length() > byteArr.length)
            throw new IndexOutOfBoundsException("Can't write " + str + " here.");

        byte[] strBytes = str.getBytes();
        for (int i = 0; i < strBytes.length; ++i)
            byteArr[start + i] = strBytes[i];
    }
}
