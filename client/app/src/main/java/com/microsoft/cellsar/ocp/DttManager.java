package com.microsoft.cellsar.ocp;

/**
 * Created by t-catayl on 9/6/2017.
 */

public class DttManager {

    private DttMessage[] dttMessages = new DttMessage[DttMessage.MAX_MESSAGE_ID];

    public DttManager() {}

    public static int parseId(byte[] buffer) {
        assert buffer.length >= 1;
        return buffer[0];
    }

    public boolean has(int id) {
        return dttMessages[id] != null;
    }

    public DttMessage get(int id) {
        return dttMessages[id];
    }

    public DttMessage add(int id) {
        if (has(id))
            remove(id);

        dttMessages[id] = new DttMessage();
        return dttMessages[id];
    }

    public void remove(int id) {
        dttMessages[id] = null;
    }
}
