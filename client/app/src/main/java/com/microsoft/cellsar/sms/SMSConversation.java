package com.microsoft.cellsar.sms;

import java.util.ArrayList;
import java.util.Collections;

/**
 * Created by t-catayl on 8/23/2017.
 */

public class SMSConversation {

    /**
     * Imsi that the pilot is holding the conversation with
     */
    private String who;
    public String getWho() { return who; }
    public void setWho(String who) { this.who = who; }

    /**
     * Collection of messages
     */
    private ArrayList<SMSMessage> messages;
    public ArrayList<SMSMessage> getMessages() {
        check();
        return messages;
    }
    public void setMessages(ArrayList<SMSMessage> messages) {
        this.messages = messages;
        sortMessaages();
    }

    /**
     * Any new messages?
     */
    private Boolean anyUnreadMessages;
    public Boolean getAnyUnreadMessages() { return anyUnreadMessages; }
    public void setAnyUnreadMessages(Boolean anyUnreadMessages) { this.anyUnreadMessages = anyUnreadMessages; }

    public SMSConversation() {}

    public SMSConversation(String who) {
        setWho(who);
        setAnyUnreadMessages(false);
    }

    public SMSMessage say(String imsi, String message) {
        check();
        if (imsi == null || (!SMS.PILOT_IMSI.equals(imsi) && !who.equals(imsi)))
            throw new IllegalArgumentException("IMSI " + imsi
                    + " does not belong to this conversation.");

        SMSMessage result = new SMSMessage(imsi, message);
        messages.add(result);

        sortMessaages();
        setAnyUnreadMessages(true);
        return result;
    }

    private void check() {
        if (this.who == null)
            throw new IllegalStateException("SMSConversation has no 'who' IMSI");
        if (this.messages == null)
            this.messages = new ArrayList<>();
        if (this.anyUnreadMessages == null)
            this.anyUnreadMessages = false;
    }

    private void sortMessaages() {
        Collections.sort(messages);
    }

}
