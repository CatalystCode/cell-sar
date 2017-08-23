package com.microsoft.cellsar.sms;

import java.util.Date;

/**
 * Created by t-catayl on 8/23/2017.
 */

public class SMSMessage implements Comparable<SMSMessage> {

    /**
     * Who sent the message
     */
    private String who;
    public String getWho() { return who; }
    public void setWho(String who) { this.who = who; }

    /**
     * Text content of the message
     */
    private String text;
    public String getText() { return text; }
    public void setText(String text) { this.text = text; }

    /**
     * Timestamp for the messaage
     */
    private Date timestamp;
    public Date getTimestamp() { return timestamp; }
    public void setTimestamp(Date timestamp) { this.timestamp = timestamp; }

    public SMSMessage() {}

    public SMSMessage(String who, String text) {
        setWho(who);
        setText(text);
        setTimestamp(new Date());
    }

    public SMSMessage(String who, String text, Date when) {
        setWho(who);
        setText(text);
        setTimestamp(when);
    }

    /**
     * Sorting is done based on time.
     * Newer messages are sorted to the beginning,
     * if either has a timestamp that is null, it is sorted to the front.
     */
    public int compareTo(SMSMessage other) {
        if (this.timestamp == null) {
            if (other.timestamp == null) return 0;
            return 1;
        }

        if (other.timestamp == null) return -1;
        return this.timestamp.compareTo(other.timestamp);
    }

    @Override
    public String toString() {
        if (text == null) return "";
        return text.trim();
    }
}
