package com.microsoft.cellsar;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.View;
import android.view.Window;
import android.view.inputmethod.InputMethodManager;
import android.widget.AbsListView;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.TextView;

import com.microsoft.cellsar.ocp.OCPClient;
import com.microsoft.cellsar.sms.SMS;
import com.microsoft.cellsar.sms.SMSConversation;

/**
 * Created by t-catayl on 8/23/2017.
 */

public class SARSMSConversationActivity extends Activity {

    private SMSConversation conversation;
    private SMSMessageArrayListAdapter msgAdapter;

    private ImageButton btnSendSMS;
    private EditText textSMSMessage;
    private ListView messageListView;
    private TextView title;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.sar_sms_conversation);

        String imsi = getIntent().getStringExtra("com.microsoft.cellsar.chatIMSI");
        this.conversation = SMS.getConversation(imsi);
        this.conversation.setAnyUnreadMessages(false);

        // set the title
        title = ((TextView) findViewById(R.id.textview_which_imsi));
        title.setText("Chatting with " + imsi);

        // setup the send button
        this.textSMSMessage = (EditText) findViewById(R.id.text_sms_message);
        this.btnSendSMS = (ImageButton) findViewById(R.id.btn_send_sms);
        this.btnSendSMS.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String message = textSMSMessage.getText().toString().trim();
                if (message.length() > 75) // must fit in the buffer
                    message = message.substring(0, 75);
                if (message == null || "".equals(message))
                    return;

                // update UI
                textSMSMessage.setText("");

                // update the conversation
                conversation.say(SMS.PILOT_IMSI, message);
                conversation.setAnyUnreadMessages(false);

                // send the SMS
                OCPClient.getInstance().sendSMS(conversation.getWho(), message);

                hideKeyboard();
            }
        });

        // set up the message list
        msgAdapter = new SMSMessageArrayListAdapter(getApplicationContext(), conversation);
        messageListView = (ListView) findViewById(R.id.lv_chat_items);
        messageListView.setAdapter(msgAdapter);

        messageListView.setTranscriptMode(AbsListView.TRANSCRIPT_MODE_ALWAYS_SCROLL);
        messageListView.setSelection(msgAdapter.getCount() - 1);
    }

    private void hideKeyboard() {
        View view = getCurrentFocus();
        if (view != null) {
            InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
            imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
        }
    }

    @Override
    protected void onStart() {
        super.onStart();

        // Since the flight controller app will be in landscape on the drone controller,
        // this is most convenient.
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    }
}
