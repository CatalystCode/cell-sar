package com.microsoft.cellsar.sms;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

/**
 * Created by t-catayl on 8/23/2017.
 */

public class SMS {

    public static final String PILOT_IMSI = "SAR_PILOT_IMSI";

    /**
     * Singleton instance of all history
     */
    private static SMS instance;
    private SMS() {}
    private static void checkInit() {
        if (instance == null) instance = new SMS();
        // instance.seed();
    }

    /**
     * Maintains a list of conversations
     */
    private HashMap<String, SMSConversation> conversations = new HashMap<>();

    public static Iterator<SMSConversation> conversationIterator() {
        checkInit();
        return instance.conversations.values().iterator();
    }

    public static SMSConversation getConversation(String imsi) {
        checkInit();
        return instance.conversations.get(imsi);
    }

    public static void startConversation(String imsi) {
        checkInit();
        if (getConversation(imsi) == null)
            instance.conversations.put(imsi, new SMSConversation(imsi));
    }

    /**
     * Seed with dummy data
     * for debugging...
     */
    private boolean seeded = false;
    public void seed() {
        if (seeded) return;
        seeded = true;

        startConversation("111222333444555");
        getConversation("111222333444555").say("111222333444555", "Hello!");

        startConversation("999888777666555");
        for (int i = 0; i < 40; ++i) {
            getConversation("999888777666555").say("999888777666555", "A Message " + i);
        }
        getConversation("999888777666555").say("999888777666555", "Newest Message");

        startConversation("012345678901234");
        startConversation("000000000000000");
        startConversation("111111111111111");
        startConversation("222222222222222");
        startConversation("333333333333333");
        startConversation("444444444444444");
        startConversation("555555555555555");

    }
}
