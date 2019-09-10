package com.jlx.utils;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.util.Log;

import java.util.Map;

/**
 * Created At 2018/5/11 17:28.
 *
 * @author larry
 */

public class USBUtils {

    private static final int VID = 1027;    //VID
    private static final int PID = 24606;     //PID

    public static void initUsbDevice(Context context) {
        UsbManager usbManager=(UsbManager) context.getSystemService(Context.USB_SERVICE);
        Intent intent=new Intent("android.hardware.usb.action.USB_DEVICE_ATTACHED");
        intent.addCategory("android.hardware.usb.action.USB_DEVICE_DETACHED");
        Map<String, UsbDevice> usbDeviceList = usbManager.getDeviceList();
        Log.e("","Init usb devices, device size = " + usbDeviceList.size() );
        if (usbDeviceList.size() > 0) {
            for (UsbDevice device : usbDeviceList.values()) {
                Log.e("","requestPression vid=" + device.getVendorId() + ",pid=" + device.getProductId());
                if (( (VID==device.getVendorId() && PID == device.getProductId()) )
                        && (!usbManager.hasPermission(device))) {
                    PendingIntent mIntent=PendingIntent.getBroadcast(context, 0, intent, 0);
                    usbManager.requestPermission(device, mIntent);
                    break;
                }

            }
        }
    }
}
