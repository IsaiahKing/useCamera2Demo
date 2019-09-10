package com.jenkolux.camerademo.camera2;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.Message;
import android.support.annotation.NonNull;
import android.text.TextUtils;
import android.util.Log;
import android.view.SurfaceHolder;

import com.jenkolux.domain.FaceData;
import com.jenkolux.lib.JlxUser;

import com.jenkolux.lib.JLXRect;

import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Callable;
import java.util.concurrent.LinkedBlockingQueue;



/**
 * 需要返回值的异步任务。
 * 
 */
public class ShowAnsyTask implements Callable<Boolean> {
    private static final String TAG = "ShowAnsyTask";
    public  static BlockingQueue<FaceData> bg =new LinkedBlockingQueue<FaceData>(1);

    private SurfaceHolder holderTransparent;
    private Paint paint     = new Paint(Paint.ANTI_ALIAS_FLAG);

    private final   Object mLock = new Object();

    public ShowAnsyTask(SurfaceHolder holderTransparent) {
        synchronized (mLock){
            this.holderTransparent = holderTransparent;
        }
    }

    @Override
    public Boolean call() throws Exception {
        while(!Thread.interrupted())
        {
            try
            {
                //Log.i(TAG, "当前的线程：" + Thread.currentThread().getName());
                //处理数据
                FaceData faceData = bg.take();
                //Log.i(TAG,"--show face rect run ---");

                handleData(faceData);//处理数据
            }
            catch (Exception e)
            {
                Log.e(TAG, "消息：" + e.getMessage());
            }
        }
        return true;
    }


    private void handleData(@NonNull FaceData faceData) {
        long timeMi = System.currentTimeMillis();
        Canvas canvas = null;
        List<JLXRect> jlxRects = faceData.jlxRects;

        try
        {
            canvas = holderTransparent.lockCanvas(null);
            if(canvas != null)
            {
                synchronized (canvas)
                {
                    canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
                    if(faceData.jlxRects == null){
                        canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);//add  clear canvas function
                    }else{
                        if(faceData.jlxRects.size() == 0){
                            canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);//add  clear canvas function
                        }

                        if(faceData.position == 0){
                            //show_WarnAlignstatus1();
                            canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);//add  clear canvas function
                            paint.setStyle(Paint.Style.STROKE);
                            for (JLXRect face : jlxRects)
                            {
                                paint.setColor(Color.RED);
                                paint.setStrokeWidth(2);
                                Rect pRt = face.getRect();
                                canvas.drawRect(pRt, paint);//画矩形框
                                Log.e(TAG,"###" + pRt.top + " bottom: " + pRt.bottom);
                            }

                        }else{
                            if(faceData.jlxRects.size() > 0){
                                paint.setStyle(Paint.Style.STROKE);
                                for (JLXRect face : jlxRects)
                                {
                                    paint.setColor(Color.GREEN);
                                    paint.setStrokeWidth(2);
                                    Rect pRt = face.getRect();
                                    canvas.drawRect(pRt, paint);//画矩形框
                                }
                            }
                        }
                    }
                }
            }else{
                Log.e(TAG,"canvas is null");
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        finally
        {
            if (canvas != null) {
                holderTransparent.unlockCanvasAndPost(canvas);
            }
        }
        //Log.i(TAG, "绘图耗时" + (System.currentTimeMillis() - timeMi));
    }


    /**
     * 比对人脸框
     */
    private boolean compareRect(Rect rect1,Rect rect2,int offset) {
        boolean ret = false;
//        if (rect1 == null || rect2 == null) {
//            return false;
//        }
        double distance = getPointDistance(rect1, rect2);
        ret = rect1 != null && rect2 != null
                && Math.abs(rect1.left - rect2.left) < offset
                && Math.abs(rect1.top - rect2.top) < offset
                && Math.abs(rect1.right - rect2.right) < offset
                && Math.abs(rect1.bottom - rect2.bottom) < offset
                && distance < offset;

        Log.e(TAG,"rect1:" + rect1
                + ",rect2:" + rect2 + ",distance = " + (int) distance);
        return ret;
    }

    /**
     * 获取两个人脸框的中心点
     * @param rect1 人脸框1
     * @param rect2 人脸框2
     * @return
     */
    private double getPointDistance(Rect rect1, Rect rect2) {
        double x1;
        double y1;
        double x2;
        double y2;
        x1 = (rect1.right + rect1.left) / 2;
        y1 = (rect1.bottom + rect1.top) / 2;
        x2 = (rect2.right + rect2.left) / 2;
        y2 = (rect2.bottom + rect2.top) / 2;
        return Math.sqrt(Math.abs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
    }

}
