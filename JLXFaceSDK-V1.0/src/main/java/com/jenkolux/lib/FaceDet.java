package com.jenkolux.lib;

import android.graphics.Bitmap;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.WorkerThread;
import android.util.Log;

import java.util.Arrays;
import java.util.List;

/**
 * Created by houzhi on 16-10-20.
 * Modified by tzutalin on 16-11-15
 */
public class FaceDet {
    private static final String TAG = "DLib";

    // accessed by native methods
    @SuppressWarnings("unused")
    private long mNativeFaceDetContext;
    private String mLandMarkPath = "";//        /mnt/sdcard/dlib/shape_predictor_68_face_landmarks.dat
    private String mFacerecogPath = "";//      /mnt/sdcard/dlib/dlib_face_recognition_resnet_model_v1.dat
    static {
        try {
            Log.d(TAG, "jniNativeClassInit loadLibrary---->start");
            System.loadLibrary("DLibAndroid");
            Log.d(TAG, "jniNativeClassInit---->start");
            jniNativeClassInit();
            Log.d(TAG, "jniNativeClassInit---->success");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "library not found");
        }
    }

    @SuppressWarnings("unused")
    public FaceDet() {
        jniInit(mLandMarkPath);
    }

    public FaceDet(String landMarkPath) {
        mLandMarkPath = landMarkPath;
        jniInit(mLandMarkPath);
    }
    public FaceDet(String landMarkPath,String recogntionModel) {
        mLandMarkPath = landMarkPath;
        mFacerecogPath = recogntionModel;
        jniInit2(mLandMarkPath,mFacerecogPath);
    }

    @Nullable
    @WorkerThread
    public List<VisionDetRet> detect(@NonNull String path) {
        VisionDetRet[] detRets = jniDetect(path);
        return Arrays.asList(detRets);
    }

    @Nullable
    @WorkerThread
    public List<VisionDetRet> detect(@NonNull Bitmap bitmap) {
        VisionDetRet[] detRets = jniBitmapDetect(bitmap);
        return Arrays.asList(detRets);
    }

    @Nullable
    @WorkerThread
    public List<VisionDetRet> detectPath(@NonNull String PicPath) {
        VisionDetRet[] detRets = jniDetect(PicPath);
        return Arrays.asList(detRets);
    }


    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        release();
    }

    public void release() {
        jniDeInit();
    }


    @Keep
    private native static void jniNativeClassInit();

    @Keep
    private synchronized native int jniInit(String landmarkModelPath);

    @Keep
    private synchronized native int jniInit2(String landmarkModelPath,String recogntionModel);

    @Keep
    private synchronized native int jniDeInit();

    @Keep
    private synchronized native VisionDetRet[] jniBitmapDetect(Bitmap bitmap);

    @Keep
    private synchronized native VisionDetRet[] jniDetect(String path);

    @Keep
    private synchronized native VisionDetRet[] jniNV21Detect(byte[] data,int width, int height);

    @Keep
    public static native byte[] rgba2Yuv(int[] data,int width, int height);

    @Keep
    public native int[] yuv420sp2rgb(byte[] data,int width, int height);


    @Keep
    public native static byte[] jniYUV2BGR(byte[] data,byte[] b_g_r,int width, int height,int[] array);

    @Keep
    public native static void jniYuvCutCropBgr(byte[] data,byte[] b_g_r,int width, int height,int[] array);


}