package com.jenkolux.facedetect;

import android.os.AsyncTask;
import android.os.Environment;
import android.util.Log;


import com.jlx.face.FaceService;
import com.jlx.status.JlxTaskBox;


public class StartEngineTask extends AsyncTask<Void,Void,Integer> {
    private  static  final String  TAG = "StartEngineTask";
    private  static  final String  SDPATH = Environment.getExternalStorageDirectory().getAbsolutePath();

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
    }

    @Override
    protected Integer doInBackground(Void... params) {
        //long StartThreadPooltimes = System.currentTimeMillis();
        int result = FaceService.JLX_FD_Init();
        if(JlxTaskBox.JLX_INITFACE_DETECT_MODEL_DONE == result){
            Log.d(TAG,"  start face detect task ");
        }
        //Log.i("startTask", "耗时: " + (System.currentTimeMillis() - StartThreadPooltimes) + " ,activeCount = " + Thread.activeCount());
        return result;
    }
}
