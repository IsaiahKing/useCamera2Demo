package com.jenkolux.util;

import android.content.Context;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.util.Log;

import java.util.HashMap;
import java.util.Map;

import static android.content.Context.USB_SERVICE;

public class UsbDeviceUtil {

    private static final String TAG = "UsbDeviceUtil";

    //默认是直连emmc
    public  static boolean isinsertDoogle = false;

    public  static boolean detect(Context context) {//该method已经在android5.1上可以使用
        HashMap<String, UsbDevice> deviceHashMap = ((UsbManager) context.getApplicationContext().getSystemService(USB_SERVICE)).getDeviceList();
        boolean check = false;
        for (Map.Entry entry : deviceHashMap.entrySet()) {
            //Log.d(TAG, "detectUsbDeviceWithUsbManager: " + entry.getKey() + ",----> " + entry.getValue());
            if (entry.getValue().toString().contains("Name=Generic")){
                if(entry.getValue().toString().contains("1507")){
                    Log.e(TAG,"detect Sg0 Devices ");
                    check = true;
                }
                break;
            }
            if (entry.getValue().toString().contains("Name=FTDI")){
                Log.e(TAG,"detect FTDI Devices ");
                check = true;
                break;
            }
            if (entry.getValue().toString().contains("Name=USB Camera")){
                Log.e(TAG,"detect USB Camera Devices ");
            }
        }
        return check;
    }

}
