package com.jlx.face;

import android.content.Context;
import android.os.AsyncTask;
import android.os.Environment;
import android.os.Message;

import android.util.Log;

import com.jenkolux.util.UsbDeviceUtil;

import com.jlx.status.JlxErrorBox;
import com.jlx.status.JlxStatusBox;
import com.jlx.status.JlxTaskBox;
import com.jlx.utils.RootCmdUtils;

import java.lang.ref.WeakReference;
import java.util.Arrays;

/**
 * gti设备引擎
 */
public class InitAiEngine extends AsyncTask<Void,Void,Integer> {
    private static final String TAG = "InitAiEngine";

    private WeakReference<Context> contextRef;
    private final Object mLock = new Object();
    private static  final String  SDPATH = Environment.getExternalStorageDirectory().getAbsolutePath();
    private static  final String  device_node_perm_path = SDPATH + "/Jlx/set_android_perm.sh";

    private static final int AI_INIT_STATUS_OK = 0;
    private static final int AI_INIT_STATUS_FAIL = -1;

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
    }

    @Override
    protected Integer doInBackground(Void... params) {
        if(prepare()){
            return AI_INIT_STATUS_OK;
        }else{
            return  AI_INIT_STATUS_FAIL;
        }
    }

    public InitAiEngine(final Context context) {
        synchronized (mLock){
            contextRef = new WeakReference<>(context);
        }
    }


    /**
     * 初始化
     * @param context 上下文
     * @return 是否成功
     */
    public boolean start(Context context){
        long     t0 = System.currentTimeMillis();
        boolean  result = false;
        //设置相似
        //Network, 0 for gNet1, 1 for gNet2
        synchronized (mLock){
            try{
                int ret;
                ret = FaceService.JLX_FR_InitEngine(context,0, 3);
                if(ret == JlxTaskBox.JLX_INITFACE_ENGINE_DONE) {
                    String[] users = FaceService.JLXgetUserList();
                    if(users != null) {
                        if (users.length == 0 || users[0] == null)//为空
                        {
                            Log.e(TAG, "InitEngine success");
                        }
                        else
                        {
                            Log.e(TAG, "InitEngine success ,time =  " + (System.currentTimeMillis() - t0) / 1000 + ",已登记人脸数：" + users.length + ",users = " + Arrays.toString(users));
                        }
                        result = true;
                    }
                    JlxStatusBox.JLX_PREDICT_STATUS = JlxStatusBox.JLX_PREDICT_STATUS_PROCESSING;
                }
                else
                {
                    Log.e(TAG, "InitEngine failure,code : "  + ret + " , time : " + (System.currentTimeMillis() - t0) / 1000);
                    JlxStatusBox.JLX_PREDICT_STATUS = JlxErrorBox.JLX_PREDICT_MODEL_ERROR;
                    JlxErrorBox.JLX_PREDICT_MODEL_ERROR_CODE = ret;
                }
            }catch (Exception e){
                e.printStackTrace();
            }
            return result;
        }
    }

    public  boolean prepare(){
        Context context = contextRef.get();
        synchronized (mLock){
            try {
                Thread.sleep(100);
                if(context != null){
                    if(UsbDeviceUtil.isinsertDoogle) {
                        //如果 插入是usb doogle, 则应该检测系统是否检测到 usb 设备
                        if (UsbDeviceUtil.detect(context)) {
                            Log.e(TAG,"detectUsbDevice ok... begin check");
                            if (RootCmdUtils.requestGtiUSBPerssion()) {
                                start(context);
                                Log.e("FaceRecognition", "InitEngine.time =  " + System.currentTimeMillis());
                                return true;
                            }
                            else {
                                Log.e(TAG, "request gti usb perssion failure");
                                return false;
                            }
                        }else{
                            Log.e(TAG,"detectUsbDevice begin check,and result error");
                            return false;
                        }
                    }else{
                        if(RootCmdUtils.requestGtiPerssion(device_node_perm_path)){
                            return start(context);
                        }
                    }
                }
            }
            catch (InterruptedException e) {
                e.printStackTrace();
            }
            return false;
        }

    }
}
