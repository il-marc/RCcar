package com.ilmarc.btcar;


import java.io.IOException;
import java.io.OutputStream;
import java.util.UUID;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.graphics.Color;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends Activity {
	
	private static final String TAG = "bluetooth1";
		
	private JoystickView joystick;
	private SeekBar seekbar;
	    
	private static final int REQUEST_ENABLE_BT = 1;
	private BluetoothAdapter btAdapter = null;
	private BluetoothSocket btSocket = null;
	private OutputStream outStream = null;
	private Timer periodicTestTimer = null; 
	private TimerTask periodicTestTimerTask = null;    
	// SPP UUID сервиса 
	private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
	 
	// MAC-адрес Bluetooth модуля
	private static String address = "00:12:06:15:14:46";
	private int speedLimit = 77;  
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //View view = this.getWindow().getDecorView();
        //view.setBackgroundColor(Color.BLACK);
        joystick = (JoystickView)findViewById(R.id.joystickView1);
        joystick.setOnJostickMovedListener(_listener);  
        joystick.setMovementRange(30);
        joystick.setClickable(false);
        seekbar = (SeekBar)findViewById(R.id.seekBar1);
        seekbar.setOnSeekBarChangeListener(_seekBarListener);
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        checkBTState();
        
    }
    private OnSeekBarChangeListener _seekBarListener = new SeekBar.OnSeekBarChangeListener() {
		
		@Override
		public void onStopTrackingTouch(SeekBar seekBar) {
			// TODO Auto-generated method stub
			
		}
		
		@Override
		public void onStartTrackingTouch(SeekBar seekBar) {
			// TODO Auto-generated method stub
			
		}
		
		@Override
		public void onProgressChanged(SeekBar seekBar, int progress,
				boolean fromUser) {
			// TODO Auto-generated method stub
			speedLimit = seekBar.getProgress()+27;
		}
	};
    private JoystickMovedListener _listener = new JoystickMovedListener() {

        @Override
        public void OnMoved(int pan, int tilt) {
        	Log.d(TAG, "Moved : " + Integer.toString(pan) + " " + Integer.toString(tilt));
            String message = new String();
            //moving
            if ( tilt == 0 ) 
            {
            	//stop
            	message += "  ";
            } else if (tilt < 0) {
            	//forward
            	message += 'w';
            	message += Character.toString((char)(Math.abs(tilt)*speedLimit/30));
            } else if (tilt > 0) {
            	//backward
            	message += 's';
            	message += Character.toString((char)(Math.abs(tilt)*speedLimit/30));
            }
            //turning
          	message += 'q';
           	message += Character.toString((char)(90+pan));
            sendData(message);             
        }

        @Override
        public void OnReleased() {
            String message = new String();
        	message += "  ";
            sendData(message);
        }
        
        public void OnReturnedToCenter() {
            String message = new String();
        	message += "  ";
            sendData(message);
        };
    };

    @Override
    public void onResume() {
    	super.onResume();
    	BluetoothDevice device = btAdapter.getRemoteDevice(address);
        
        // Two things are needed to make a connection:
        //   A MAC address, which we got above.
        //   A Service ID or UUID.  In this case we are using the
        //     UUID for SPP.
        try {
          btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
        } catch (IOException e) {
          errorExit("Fatal Error", "In onResume() and socket create failed: " + e.getMessage() + ".");
        }
        
        // Discovery is resource intensive.  Make sure it isn't going on
        // when you attempt to connect and pass your message.
        btAdapter.cancelDiscovery();
        
        // Establish the connection.  This will block until it connects.
        Log.d(TAG, "...Соединяемся...");
        try {
	        btSocket.connect();
	        Log.d(TAG, "...Соединение установлено и готово к передачи данных...");
	        periodicTestTimer = new java.util.Timer();
	        periodicTestTimerTask = new TimerTask() {
		        public void run()
		        {
		        	sendData("vv");
		        	Log.d(TAG, "Test sent");
		        }
	        };
	        periodicTestTimer.schedule(periodicTestTimerTask, 5000, 500);
        } catch (IOException e) {
          try {
            btSocket.close();
          } catch (IOException e2) {
            errorExit("Fatal Error", "In onResume() and unable to close socket during connection failure" + e2.getMessage() + ".");
          }
        }
          
        // Create a data stream so we can talk to server.
        Log.d(TAG, "...Создание Socket...");
      
        try {
          outStream = btSocket.getOutputStream();
        } catch (IOException e) {
          errorExit("Fatal Error", "In onResume() and output stream creation failed:" + e.getMessage() + ".");
        }
    }
    
    @Override
    public void onPause() {
      super.onPause();
    
      Log.d(TAG, "...In onPause()...");
    
      if (outStream != null) {
        try {
          outStream.flush();
        } catch (IOException e) {
          errorExit("Fatal Error", "In onPause() and failed to flush output stream: " + e.getMessage() + ".");
        }
      }
    
      try     {
        btSocket.close();
      } catch (IOException e2) {
        errorExit("Fatal Error", "In onPause() and failed to close socket." + e2.getMessage() + ".");
      }
    }
      
    private void checkBTState() {
      // Check for Bluetooth support and then check to make sure it is turned on
      // Emulator doesn't support Bluetooth and will return null
      if(btAdapter==null) { 
        errorExit("Fatal Error", "Bluetooth Not supported. Aborting.");
      } else {
        if (!btAdapter.isEnabled()) {
          //Prompt user to turn on Bluetooth
          Intent enableBtIntent = new Intent(btAdapter.ACTION_REQUEST_ENABLE);
          startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
      }
    }
    
    private void errorExit(String title, String message){
      Toast.makeText(getBaseContext(), title + " - " + message, Toast.LENGTH_LONG).show();
      finish();
    }
    
    private void sendData(String message) {
      byte[] msgBuffer = message.getBytes();
    
      //Log.d(TAG, "...Посылаем данные: " + message + "...");
    
      try {
        outStream.write(msgBuffer);
      } catch (IOException e) {
        String msg = "In onResume() and an exception occurred during write";
        errorExit("Fatal Error", msg);       
      }
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        // getMenuInflater().inflate(R.menu.main, menu);
        return false;
    }
    
    
}
