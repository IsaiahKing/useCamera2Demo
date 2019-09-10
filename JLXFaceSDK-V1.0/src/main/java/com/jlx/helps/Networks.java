package com.jlx.helps;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;

import com.jlx.status.JLXNetStatusBox;

public class Networks {
    private static final Networks ourInstance = new Networks();
    private static final String TAG = "Networks";
    public static Networks getInstance() {
        return ourInstance;
    }

    private Networks() {
    }

    //得到网络类型
    public  int getstates(Context context){
        ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetworkInfo = connectivityManager.getActiveNetworkInfo();
        if (activeNetworkInfo == null || !activeNetworkInfo.isConnected()) {
            return JLXNetStatusBox.None;
        }

        int type = activeNetworkInfo.getType();
        switch (type) {
            case ConnectivityManager.TYPE_MOBILE:
                return JLXNetStatusBox.YiDong;//移动数据
            case ConnectivityManager.TYPE_WIFI:
                return JLXNetStatusBox.WIFI;//WIFI
            case ConnectivityManager.TYPE_ETHERNET:
                return JLXNetStatusBox.local;//本地
            default:
                break;
        }
        return JLXNetStatusBox.None;
    }

    //网络连接是否可用...
    public  boolean isConnectionAvailable(Context cotext)
    {
        boolean isConnectionFail = true;
        ConnectivityManager connectivityManager = (ConnectivityManager)cotext.getSystemService(Context.CONNECTIVITY_SERVICE);
        if (connectivityManager != null)
        {
            NetworkInfo activeNetworkInfo = connectivityManager.getActiveNetworkInfo();
            if (activeNetworkInfo == null || !activeNetworkInfo.isConnected())
            {
                isConnectionFail = true;
            }
            else
            {
                isConnectionFail = false;
            }
        }
        else
        {
            Log.e(TAG, "Can't get connectivitManager");
        }
        return isConnectionFail;
    }


}
