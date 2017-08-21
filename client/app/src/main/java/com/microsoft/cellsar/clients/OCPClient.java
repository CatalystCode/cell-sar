package com.microsoft.cellsar.clients;

import android.util.Log;

import dji.common.error.DJIError;
import dji.common.util.CommonCallbacks;
import dji.sdk.flightcontroller.FlightController;

/**
 * Created by t-catayl on 8/21/2017.
 */

public class OCPClient {

    private FlightController flightController;

    private Boolean plmn310410 = true;

    public OCPClient() {
        this.flightController = null;
    }

    public OCPClient(FlightController flightController) {
        this.flightController = flightController;
    }

    public void togglePMLN() {
        byte[] data;
        if (this.plmn310410) {
            Log.v("ocp", "toggling PLMN to 311 480");
            data = buildPLMNPacket("311", "480");
        } else {
            Log.v("ocp", "toggling PLMN to 310 410");
            data = buildPLMNPacket("310", "410");
        }

        Log.v("ocp", "plmn packet built. sending...");
        this.sendData(data, "plmn");
    }

    public void sayHi(String imsi) {
        byte[] data = buildSMSPacket(imsi, "hakuna matata");
        Log.v("ocp", "sms packet built. sending...");
        sendData(data, "sms");
    }

    private void sendData(byte[] data, final String type) {
        if (this.flightController == null)
            return;

        this.flightController.sendDataToOnboardSDKDevice(data, new CommonCallbacks.CompletionCallback() {
            @Override
            public void onResult(DJIError djiError) {
                Log.v("ocp", "OCPClient.sendData (type=" + type + " completed: " + djiError.getDescription());
            }
        });
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
