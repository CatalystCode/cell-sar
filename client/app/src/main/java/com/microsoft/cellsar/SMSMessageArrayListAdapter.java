package com.microsoft.cellsar;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.microsoft.cellsar.sms.SMS;
import com.microsoft.cellsar.sms.SMSConversation;
import com.microsoft.cellsar.sms.SMSMessage;

import java.util.ArrayList;

/**
 * Created by t-catayl on 8/23/2017.
 */

public class SMSMessageArrayListAdapter extends BaseAdapter {

    private SMSConversation conversation;
    private LayoutInflater inflater;

    public SMSMessageArrayListAdapter(Context ctx, SMSConversation conversation) {
        this.conversation = conversation;
        inflater = LayoutInflater.from(ctx);
    }

    @Override
    public int getCount() {
        return conversation.getMessages().size();
    }

    @Override
    public Object getItem(int i) {
        return conversation.getMessages().get(i).toString();
    }

    @Override
    public long getItemId(int i) {
        return i;
    }

    @Override
    public View getView(int i, View view, ViewGroup viewGroup) {
        SMSMessage msg = conversation.getMessages().get(i);
        View result;
        if (SMS.PILOT_IMSI.equals(msg.getWho()))
            result = inflater.inflate(R.layout.sms_message_us, null);
        else
            result = inflater.inflate(R.layout.sms_message_them, null);

        ((TextView) result).setText((String)this.getItem(i));
        return result;
    }
}
