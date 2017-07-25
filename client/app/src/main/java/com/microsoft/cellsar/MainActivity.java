package com.microsoft.cellsar;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.Circle;
import com.google.android.gms.maps.model.CircleOptions;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polygon;
import com.google.android.gms.maps.model.PolygonOptions;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import dji.common.flightcontroller.FlightControllerState;
import dji.common.flightcontroller.LocationCoordinate3D;
import dji.sdk.base.BaseProduct;
import dji.sdk.flightcontroller.FlightController;
import dji.sdk.products.Aircraft;

import static android.R.attr.data;

/** Main activity that displays two choices to user */
public class MainActivity extends FragmentActivity implements GoogleMap.OnMapClickListener, OnMapReadyCallback {
    private GoogleMap gMap;
    private double droneLocationLat = 181, droneLocationLng = 181;
    private Marker droneMarker = null;
    private FlightController mFlightController;

    protected BroadcastReceiver mReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            onProductConnectionChange();
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_default_widgets);

        //Register BroadcastReceiver to find out when drone connection status changes
        IntentFilter filter = new IntentFilter();
        filter.addAction(Mobile.FLAG_CONNECTION_CHANGE);
        registerReceiver(mReceiver, filter);

        initUI();

        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.mapView);
        mapFragment.getMapAsync(this);


    }

    private void initUI() {

        // Configure buttons and stuff here, e.g.
        // locate = (Button) findViewById(R.id.locate);
        // locate.setOnClickListener(this);

    }

    @Override
    protected void onResume() {
        super.onResume();

        // Hide both the navigation bar and the status bar.
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

        initFlightController();
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
                    try {
                        // do something with incoming OSDK data
                        final String msg =  new String(bytes);
                        Log.v("OnboardSDKDevice", msg);

                        // Is this a STATUS or an IR (IMSI/RSSI) message?
                        if (msg.startsWith("!"))
                        {
                            // STATUS message
                            setResultToToast(msg);
                        }
                        else {
                            // IR message
                            String[] parts = msg.split("_");
                            String imsi = parts[0];
                            String upRssi = parts[1];
                            Integer upRssiInt = Integer.parseInt(upRssi);
                            drawSignal(upRssiInt, droneLocationLat, droneLocationLng);
                        }
                    }
                    catch (Throwable e)
                    {
                        Log.v("OnboardSDKDevice", "failed parsing message");
                    }
                }
            });
        }
    }

    public static boolean checkGpsCoordinates(double latitude, double longitude) {
        return (latitude > -90 && latitude < 90 && longitude > -180 && longitude < 180) && (latitude != 0f && longitude != 0f);
    }

    private HashMap<Integer, Polygon> scannedGrid = new HashMap<>();

    private void drawSignal(int signal, double lat, double lon) {
        if (checkGpsCoordinates(lat, lon)) {
            // round lat/long to 0.0001 for roughly 10 meter grids
            final LatLng pos = new LatLng(Math.round(lat * 10000d)/10000d, Math.round(lon * 10000d)/10000d);

            final int signalColor;
            if (signal > -20)
            {
                signalColor = Color.argb(100, 0, 255, 0);
            }
            else if (signal > -50)
            {
                signalColor = Color.argb(100, 250, 220, 50);
            }
            else
            {
                signalColor = Color.argb(100, 255, 0, 0);
            }

            // Check if we already have a polygon here, if so, remove it.
            if (scannedGrid.containsKey(pos.hashCode()))
            {
                scannedGrid.get(pos.hashCode()).remove();
            }

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
