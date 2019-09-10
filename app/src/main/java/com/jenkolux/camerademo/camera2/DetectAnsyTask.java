package com.jenkolux.camerademo.camera2;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.os.SystemClock;
import android.util.Log;


import com.jenkolux.camerademo.camera2.activity.MainActivity;
import com.jenkolux.domain.FaceData;
import com.jenkolux.facedetect.data.JlxBox;
import com.jenkolux.util.JlxScreenUtil;
import com.jlx.face.FaceService;
import com.jlx.helps.FaceHelper;
import com.jlx.status.JlxTaskBox;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Callable;
import java.util.concurrent.LinkedBlockingQueue;



/**
 * 需要返回值的异步任务。
 * 
 */
public class DetectAnsyTask implements Callable<Boolean> {
    private static final String TAG = "DetectAnsyTask";
    public  static BlockingQueue<byte[]> bg = new LinkedBlockingQueue<>(1);
    public  static byte[] checkData;
    private final   Object mLock = new Object();
    private WeakReference<Context> contextRef;

    private static long detect_has_noface_num = 0;
    private static long detect_has_face_num = 0;

    public DetectAnsyTask(Context context){
        synchronized (mLock){
            contextRef = new WeakReference<>(context);
            JlxTaskBox.JLX_THREADPOOL_TASK_STATUS = JlxTaskBox.JLX_THREADPOOL_TASK_DONE;
        }
    }

    private void method(final byte[] nv21){
        if((detect_has_noface_num % 10 == 0)){
            //连续采集30张都有人脸,那么系统将延时10ms
            if(detect_has_face_num == 30){
                detect_has_face_num = 0;
                SystemClock.sleep(60);
                return;
            }

            //Log.i(TAG,"SCREEN width  : " + MainActivity.SCREEN_WIDTH);
            //Log.i(TAG,"SCREEN height : " + MainActivity.SCREEN_HEIGHT);

            ArrayList<JlxBox>   jlxBoxes = FaceService.detectFaceAlign(nv21, JlxScreenUtil.getPreWidth(), JlxScreenUtil.getPreHeight());
            FaceData            faceData = new FaceData();
            if(jlxBoxes != null){
                if(jlxBoxes.size() > 0){
                    detect_has_face_num++;
                    Log.i(TAG,"detect_has_face_num ++++");

                    //获取当前第一个
                    JlxBox          face_every_data = jlxBoxes.get(0);

                    faceData.jlxRects = FaceHelper.getInstance().scaleRect(jlxBoxes, MainActivity.SCREEN_WIDTH,MainActivity.SCREEN_HEIGHT);
                    faceData.position = face_every_data.getAlignface();

                    //将　faceData　显示（人脸框）
                    ShowAnsyTask.bg.offer(faceData);
                }
            }else{
                //Log.i(TAG,"detect_has_face_num ----");
                ShowAnsyTask.bg.offer(faceData);
                //add
                if(detect_has_noface_num %3 == 0){
                    SystemClock.sleep(6);
                }
                if(detect_has_noface_num %5 == 0){
                    SystemClock.sleep(8);
                }
                detect_has_noface_num += 1;
            }
        }else{
            detect_has_noface_num += 1;
        }
        if(detect_has_noface_num >= Integer.MAX_VALUE){
            detect_has_noface_num = 0;
        }
        if(detect_has_face_num >= 100){
            detect_has_face_num = 0;
        }
    }

    @Override
    public Boolean call() throws Exception {
        while (!Thread.interrupted()) {
            try {
                //long start = System.currentTimeMillis();
                byte[] nv21 = bg.take();
                method(nv21);
            } catch (Exception e) {
                Log.e(TAG, "Detect Task 消息：" + e.getMessage());
            }
        }
        return true;
    }
}
