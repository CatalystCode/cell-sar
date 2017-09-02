package com.microsoft.cellsar;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.microsoft.cellsar.ocp.OCP;
import com.microsoft.cellsar.sms.SMS;
import com.microsoft.cellsar.sms.SMSConversation;
import com.microsoft.cellsar.sms.SMSMessage;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Iterator;

/**
 * Created by t-catayl on 8/23/2017.
 */

public class SARSMSActivity extends Activity {
    private static final String TAG = "SARSMS";
    private static final int PREVIEW_LENGTH = 32;

    private OCP ocp;

    private LinearLayout conversationTable;
    private ArrayList<ConversationCell> conversationCells;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.sar_sms);

        conversationTable = (LinearLayout) findViewById(R.id.layout_sms_conversations);
        conversationCells = new ArrayList<>();
        Iterator<SMSConversation> conversations = SMS.conversationIterator();
        while (conversations.hasNext())
            addConversationToView(conversations.next());
    }

    private void addConversationToView(SMSConversation c) {
        final View cellView = LayoutInflater.from(this).inflate(R.layout.sms_conversation_cell, conversationTable, false);

        updateConversationCell(c, cellView);

        // React to clicking on it
        cellView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent convIntent = new Intent(getApplicationContext(), SARSMSConversationActivity.class);
                String imsi = ((TextView) view.findViewById(R.id.sms_cell_imsi)).getText().toString().trim();
                convIntent.putExtra("com.microsoft.cellsar.chatIMSI", imsi);
                startActivity(convIntent);
            }
        });

        conversationTable.addView(cellView);
        conversationCells.add(new ConversationCell(c, cellView));
    }

    private void updateConversationCell(SMSConversation c, View cellView) {
        SMSMessage lastMsg = null;
        if (c.getMessages() != null && c.getMessages().size() > 0)
            lastMsg = c.getMessages().get(c.getMessages().size() - 1);

        // IMSI
        ((TextView) cellView.findViewById(R.id.sms_cell_imsi)).setText(c.getWho());

        // Preview
        String preview = "";
        if (lastMsg != null)
            preview = lastMsg.getText().length() + 3 > PREVIEW_LENGTH ?
                    lastMsg.getText().substring(0, PREVIEW_LENGTH - 3) + "..." :
                    lastMsg.getText();
        ((TextView) cellView.findViewById(R.id.sms_cell_preview)).setText(preview);

        // Unread notification
        cellView.findViewById(R.id.sms_cell_unread).setVisibility(Boolean.TRUE.equals(c.getAnyUnreadMessages()) ? View.VISIBLE : View.INVISIBLE);

        // Date
        SimpleDateFormat sdf = new SimpleDateFormat("HH:mm");
        String dateStr = lastMsg == null ? "" : sdf.format(lastMsg.getTimestamp());
        ((TextView) cellView.findViewById(R.id.sms_cell_time)).setText(dateStr);

    }

    @Override
    protected void onStart() {
        super.onStart();

        // Since the flight controller app will be in landscape on the drone controller,
        // this is most convenient.
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    }

    @Override
    protected void onResume() {
        super.onResume();

        for (ConversationCell cc : conversationCells)
            updateConversationCell(cc.conversation, cc.cell);
    }

    private class ConversationCell {
        public View cell;
        public SMSConversation conversation;

        public ConversationCell(SMSConversation smsConversation, View cell) {
            this.cell = cell;
            this.conversation = smsConversation;
        }
    }
}
