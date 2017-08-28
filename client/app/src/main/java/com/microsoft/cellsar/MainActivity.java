package com.microsoft.cellsar;

import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.nfc.FormatException;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polygon;
import com.google.android.gms.maps.model.PolygonOptions;
import com.microsoft.cellsar.ocp.OCPClient;
import com.microsoft.cellsar.sms.SMS;
import com.microsoft.cellsar.sms.SMSConversation;

import java.util.Collections;
import java.util.HashMap;
import java.util.IllegalFormatException;
import java.util.Map;

import dji.common.flightcontroller.FlightControllerState;
import dji.common.flightcontroller.LocationCoordinate3D;
import dji.sdk.base.BaseProduct;
import dji.sdk.flightcontroller.FlightController;
import dji.sdk.products.Aircraft;
import dji.ui.widget.FPVWidget;

/** Main activity that displays two choices to user */
public class MainActivity extends FragmentActivity implements GoogleMap.OnMapClickListener, OnMapReadyCallback {
    private GoogleMap gMap;
    private double droneLocationLat = 181, droneLocationLng = 181;
    private Marker droneMarker = null;
    private FlightController mFlightController;
    private Button logToggle;
    private FPVWidget fpvWidget;
    private TextView txtLog;
    private Button btnSAR;
    private ScrollView scrollMap;
    SupportMapFragment mapFragment;

    protected BroadcastReceiver mReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            onProductConnectionChange();
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.mainactivity);

        //Register BroadcastReceiver to find out when drone connection status changes
        IntentFilter filter = new IntentFilter();
        filter.addAction(Mobile.FLAG_CONNECTION_CHANGE);
        registerReceiver(mReceiver, filter);

        initUI();

        mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.mapView);
        mapFragment.getMapAsync(this);
    }

    private void initUI() {

        // Configure buttons and stuff here, e.g.
        // locate = (Button) findViewById(R.id.locate);
        // locate.setOnClickListener(this);

        fpvWidget = (FPVWidget)findViewById(R.id.FPVWidget);
        fpvWidget.bringToFront();


        // setup log
        logToggle = (Button)findViewById(R.id.btnLog);
        txtLog  = (TextView)findViewById(R.id.txtLog);
        txtLog.setMovementMethod(new ScrollingMovementMethod());
        logToggle.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                 txtLog.setVisibility(txtLog.getVisibility() == View.VISIBLE ? View.INVISIBLE : View.VISIBLE);
            }
        });

        // setup SAR button
        btnSAR = (Button)findViewById(R.id.btn_sar);
        btnSAR.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                final CharSequence items[] = new CharSequence[] {"Change PLMN", "SMS Messaging"};
                new AlertDialog.Builder(btnSAR.getContext())
                    .setTitle("Search And Rescue")
                    .setItems(items, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int i) {
                            switch (i) {
                                case 0: // Change PLMN
                                    startChangePLMNDialog();
                                    break;
                                case 1: // SMS Messaging
                                    Intent smsIntent = new Intent(getApplicationContext(), SARSMSActivity.class);
                                    startActivity(smsIntent);
                                    break;
                            }
                        }
                    }).setOnDismissListener(new DialogInterface.OnDismissListener() {
                        @Override
                        public void onDismiss(DialogInterface dialogInterface) {
                            hideNavAndStatus();
                        }
                    }).show();
            }
        });
    }

    private void startChangePLMNDialog() {
        LayoutInflater li = LayoutInflater.from(getApplicationContext());
        final View plmnView = li.inflate(R.layout.plmn_change, null);

        new AlertDialog.Builder(this)
            .setMessage("Set the new PLMN").setView(plmnView)
            .setNegativeButton("Cancel", null)
            .setPositiveButton("Apply", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {

                    String mcc = parsePLMNFragment((EditText)plmnView.findViewById(R.id.txt_plmn_mcc));
                    String mnc = parsePLMNFragment((EditText)plmnView.findViewById(R.id.txt_plmn_mnc));
                    OCPClient.getInstance().setPLMN(mcc, mnc);

                    String message = "Set PLMN to " + mcc + mnc;
                    Toast.makeText(getApplicationContext(), message, Toast.LENGTH_LONG).show();
                }
            })
        .show();
    }

    private String parsePLMNFragment(EditText et) {
        String result = et.getText().toString();
        if (result.length() != 3) {
            try {
                result = String.format("%03d", Integer.parseInt(result));
            } catch (NumberFormatException nfe) {
                result = "000";
            }
        }

        return result;
    }

    private void hideNavAndStatus() {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

    @Override
    protected void onResume() {
        super.onResume();

        // Hide both the navigation bar and the status bar.
        hideNavAndStatus();

        if (this.mFlightController == null) {
            initFlightController();
            OCPClient.getInstance().setFlightController(this.mFlightController);
        }
    }

    @Override
    protected void onPause(){
        super.onPause();
    }

    @Override
    protected void onDestroy(){
        unregisterReceiver(mReceiver);
        super.onDestroy();
    }

    private void setResultToToast(final String string){
        MainActivity.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.this, string, Toast.LENGTH_SHORT).show();
            }
        });
    }

    private void writeToConsole(final String string){
        MainActivity.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                txtLog.append(string + "\n");
            }
        });
    }

    @Override
    public void onMapReady(GoogleMap googleMap) {
        if (gMap == null) {
            gMap = googleMap;
            setUpMap();
            cameraUpdate(new LatLng(47.639566, -122.129070));

        }
    }

    private void setUpMap() {
        gMap.setOnMapClickListener(this);// add the listener for click for amap object
        gMap.setMapType(GoogleMap.MAP_TYPE_NORMAL);
    }

    private void cameraUpdate(LatLng pos){
        float zoomlevel = (float) 18.0;
        CameraUpdate cu = CameraUpdateFactory.newLatLngZoom(pos, zoomlevel);
        gMap.moveCamera(cu);

    }

    @Override
    public void onMapClick(LatLng point) {
    }

    private void onProductConnectionChange()
    {
        initFlightController();
    }
    private void initFlightController() {
        BaseProduct product = Mobile.getProductInstance();
        if (product != null && product.isConnected()) {
            if (product instanceof Aircraft) {
                mFlightController = ((Aircraft) product).getFlightController();
            }
        }
        if (mFlightController != null) {
            mFlightController.setStateCallback(new FlightControllerState.Callback() {
                @Override
                public void onUpdate(FlightControllerState djiFlightControllerCurrentState) {
                    LocationCoordinate3D location = djiFlightControllerCurrentState.getAircraftLocation();
                    Log.v("GPS", location.toString());

                    droneLocationLat = location.getLatitude();
                    droneLocationLng = location.getLongitude();
                    updateDroneLocation();

                }
            });

            mFlightController.setOnboardSDKDeviceDataCallback(new FlightController.OnboardSDKDeviceDataCallback() {
                @Override
                public void onReceive(byte[] bytes) {
                    if (!oldHandleOCPMessage(bytes))
                        newHandleOCPMessage(bytes);
                }
            });
        }
    }

    public boolean oldHandleOCPMessage(byte[] bytes) {
        try {
            // do something with incoming OSDK data
            final String msg =  new String(bytes);
            Log.v("OnboardSDKDevice", msg);

            // Is this a STATUS or an IR (IMSI/RSSI) message?
            if (msg.startsWith("!")) {
                // STATUS message
                writeToConsole(msg.substring(1));
                return true;
            } else {
                // IR message
                String[] parts = msg.split("_");
                if (parts.length != 2)
                    throw new FormatException();
                String imsi = parts[0];
                SMS.startConversation(imsi);
                String upRssi = parts[1];
                Integer upRssiInt = Integer.parseInt(upRssi);
                drawSignal(upRssiInt, droneLocationLat, droneLocationLng);
                return true;
            }
        } catch (Throwable e) {
            Log.v("OnboardSDKDevice", "failed parsing message");
            return false;
        }
    }

    public void newHandleOCPMessage(byte[] bytes) {
        try {
            String str = new String(bytes);

            // check if first 10 bytes are the sms header
            if (str.startsWith("sms")) {
                final String imsi = str.substring(10, 25);
                final String msg = str.substring(25, str.length());

                SMS.startConversation(imsi);
                SMSConversation conversation = SMS.getConversation(imsi);
                conversation.say(imsi, msg);

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(getApplicationContext(), "New SMS from " + imsi, Toast.LENGTH_LONG).show();
                        SMSMessageArrayListAdapter chat = OCPClient.getInstance().chatAdapter;
                        if (chat != null)
                            chat.notifyDataSetChanged();
                    }
                });
            }
        } catch (Throwable e) {
            Log.v("OnboardSDKDevice", "failed parsing message the second time...");
        }
    }

    public static boolean checkGpsCoordinates(double latitude, double longitude) {
        return (latitude > -90 && latitude < 90 && longitude > -180 && longitude < 180) && (latitude != 0f && longitude != 0f);
    }

    private HashMap<Integer, Integer> scannedGridSignal = new HashMap<>();
    private HashMap<Integer, Polygon> scannedGrid = new HashMap<>();

    private void drawSignal(int signal, double lat, double lon) {
        if (checkGpsCoordinates(lat, lon)) {
            // round lat/long to 0.0001 for roughly 10 meter grids
            final LatLng pos = new LatLng(Math.round(lat * 10000d)/10000d, Math.round(lon * 10000d)/10000d);

            // Set RSSI and get color ranges
            scannedGridSignal.put(pos.hashCode(), signal);

            int minSignal = Collections.min(scannedGridSignal.values());
            int maxSignal = Collections.max(scannedGridSignal.values());
            
            // Update all old polygons
            for(Map.Entry<Integer, Polygon> entry : scannedGrid.entrySet()) {
                Integer k = entry.getKey();
                final Polygon v = entry.getValue();

                final int newColor = getSignalColor(scannedGridSignal.get(k), minSignal, maxSignal);

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        v.setFillColor(newColor);
                    }
                });
            }

            // Check if we already have a polygon here, if not, add it
            if (!scannedGrid.containsKey(pos.hashCode()))
            {
                final int signalColor;
                signalColor = getSignalColor(signal, minSignal, maxSignal);

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        scannedGrid.put(pos.hashCode(), gMap.addPolygon(new PolygonOptions()
                                .add(
                                        pos,
                                        new LatLng(pos.latitude + 0.0001, pos.longitude),
                                        new LatLng(pos.latitude + 0.0001, pos.longitude + 0.0001),
                                        new LatLng(pos.latitude, pos.longitude + 0.0001))
                                .strokeWidth(0)
                                .fillColor(signalColor)
                        ));
                    }
                });
            }
        }
    }

    private int getSignalColor(int signal, int minSignal, int maxSignal) {
        int signalColor;
        float normalizedSignal = (float)(signal - minSignal)/(float)(maxSignal - minSignal);

        if (normalizedSignal <= 0.25)
        {
            signalColor = Color.parseColor("#64f26522");
        }
        else if (normalizedSignal <= 0.60)
        {
            signalColor = Color.parseColor("#64fcdc33");
        }
        else if (normalizedSignal <= 0.80)
        {
            signalColor = Color.parseColor("#6433ad3d");
        }
        else {
            signalColor = Color.parseColor("#6400ff14");
        }

        return signalColor;
    }

    private void updateDroneLocation(){
        final LatLng pos = new LatLng(droneLocationLat, droneLocationLng);
        //Create MarkerOptions object
        final MarkerOptions markerOptions = new MarkerOptions();
        markerOptions.position(pos);
        markerOptions.icon(BitmapDescriptorFactory.fromResource(R.drawable.aircraft));
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (droneMarker != null) {
                    droneMarker.remove();
                }
                if (checkGpsCoordinates(droneLocationLat, droneLocationLng)) {
                    droneMarker = gMap.addMarker(markerOptions);
                    cameraUpdate(pos);
                }
            }
        });
    }



}
