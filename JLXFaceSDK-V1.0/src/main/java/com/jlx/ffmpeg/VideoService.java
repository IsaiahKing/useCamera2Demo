package com.jlx.ffmpeg;

import android.util.Log;
import android.view.Surface;

public class VideoService{
    private static final String  TAG  = "VideoService";

//    static {
//        try{
//            System.loadLibrary("avcodec");
//            System.loadLibrary("avdevice");
//            System.loadLibrary("avfilter");
//            System.loadLibrary("avformat");
//            System.loadLibrary("avutil");
//            System.loadLibrary("postproc");
//            System.loadLibrary("swscale");
//            System.loadLibrary("ffmpeg_android");
//            Log.d(TAG, "Loading ffmpeg library.");
//        } catch (Exception ignored) {
//            Log.d(TAG, "Loading ffmpeg library cause an Exception.");
//        }
//    }
//    public VideoService(){
//        initffmpeg();
//    }
//    public static native void   initffmpeg();
//    public static native int    openvideo(String name,Surface surface);//显示在surface上
//    public static native void   Deinit();
//    public static native byte[] getnv21();//返回一帧图片
//    public static native int    getwidth();//得到宽
//    public static native int    getheight();//得到高
//    public static native boolean getvideostatus();//视频打开状态
//    public static native void    stop();//停止线程..
}
