package com.microsoft.cellsar.ocp;

import android.util.Log;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.Message;
import com.microsoft.cellsar.SMSMessageArrayListAdapter;
import com.microsoft.cellsar.protobuf.fc.FromFc.PlmnData;
import com.microsoft.cellsar.protobuf.fc.FromFc.SmsData;
import com.microsoft.cellsar.protobuf.Data.Subscriber;
import com.microsoft.cellsar.protobuf.yate.FromYate.YateMessage;

import dji.sdk.flightcontroller.FlightController;

/**
 * Created by t-catayl on 8/21/2017.
 */

public class OCP {
    private static final String TAG = "OCP";

    private FlightController flightController;
    public SMSMessageArrayListAdapter chatAdapter;

    /* Singleton Instance */
    private static OCP instance;
    private OCP() {}
    public static OCP getInstance() {
        if (instance == null)
            instance = new OCP();
        return instance;
    }

    public void setFlightController(FlightController fc) {
        this.flightController = fc;
    }

    public void setPLMN(String mcc, String mnc) {
        sendMessage(
            PlmnData.newBuilder()
            .setMcc(mcc)
            .setMnc(mnc)
            .build()
        );
    }

    public void sendSMS(String imsi, String message) {
        sendMessage(
            SmsData.newBuilder()
            .setSubscriber(Subscriber.newBuilder()
                .setImsi(imsi)
                .build())
            .setMsg(message)
            .setTries(3)
            .build()
        );
    }

    private void sendMessage(Message m) {
        if (this.flightController == null)
            return;

        this.flightController.sendDataToOnboardSDKDevice(m.toByteArray(), null);
    }

    public static YateMessage parseBytes(byte[] bytes) {
        try {
            return YateMessage.parseFrom(bytes);
        } catch (InvalidProtocolBufferException e) {
            Log.e(TAG, "Unable to parse YateMessage: " + e.getMessage());
            return null;
        }
    }

}
