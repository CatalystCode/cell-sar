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
    private static DttManager dttManager = new DttManager();
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

        DttMessage dttMessage = new DttMessage(m.toByteArray());
        while (dttMessage.hasNextPacket())
            this.flightController.sendDataToOnboardSDKDevice(dttMessage.nextPacket(), null);
    }

    public static YateMessage parseBytes(byte[] bytes) {
        try {

            Log.i(TAG, "Inspecting packet from Yate");
            int id = DttManager.parseId(bytes);
            DttMessage dttMsg = dttManager.has(id) ? dttManager.get(id) : dttManager.add(id);

            boolean good = dttMsg.addPacket(bytes);
            if (!good) {
                dttMsg = dttManager.add(id);
                good = dttMsg.addPacket(bytes);
            }

            if (!good) {
                Log.w(TAG, "Could not make sense of the packet...");
                return null;
            } else if (dttMsg.isComplete()) {
                return YateMessage.parseFrom(dttMsg.getWholeMessage());
            }

        } catch (Exception e) {
            Log.e(TAG, "Unable to parse YateMessage: " + e.getMessage());
        }
        return null;
    }

}
