package com.jlx.face;

import android.content.Context;
import android.os.AsyncTask;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.Log;

import com.jenkolux.facedetect.data.JlxBox;
import com.jenkolux.filessl.FileUtil;
import com.jenkolux.filessl.JlxEncrypt;
import com.jenkolux.lib.JLXPersonInfo;
import com.jenkolux.util.DeviceIdUtil;
import com.jlx.helps.Networks;
import com.jlx.status.JLXNetStatusBox;
import com.jlx.status.JlxStatusBox;
import com.jlx.status.JlxTaskBox;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.sql.Array;
import java.util.ArrayList;
import java.util.List;

/**
 * Created At 2019/5/1 12:00.
 *
 * @author larry,leger
 */

public class FaceService {
    private static final String TAG = "FaceService";
    private static final String MainJlxDictory = "Jlx";
    private static final String SDPATH = Environment.getExternalStorageDirectory().getAbsolutePath();

    private static final String deviceInfo = "dstfile.txt";
    private static boolean active_status = true;
    private static final String MainMtcnnDictory = "mtcnn";

    private static  final String ModelPath = SDPATH + File.separator +
            MainJlxDictory + File.separator + MainMtcnnDictory + "/models";

    private static  final String SSLPath =SDPATH + File.separator + MainJlxDictory + File.separator + JlxEncrypt.desFileName;

    static {
        try {
            System.loadLibrary("tensorflowlite");
            System.loadLibrary("jlx_classify");
            //Log.d(TAG, "Loading jlx_classify library.");
        } catch (UnsatisfiedLinkError error) {
            Log.e(TAG, "Load library error :" + error);
        } catch (Exception ignored) {
            //Log.e(TAG, "Loading jlx_classify library cause an Exception.");
        }
    }

    /**
     *
     * @return
     */
    public static int JLX_FD_Init(){
        return initdetectModel(ModelPath);
    }

    /**
     * 释放人脸检测相关
     * @return
     */
    public static boolean JLX_FD_Release(){
        return releseModel();
    }

    public static native synchronized ArrayList<JlxBox> detectFace(@NonNull byte[] nv21, int with, int heigth);

    public static native synchronized ArrayList<JlxBox> detectMaxFace(@NonNull byte[] nv21, int with, int heigth);

    public static native  synchronized ArrayList<JlxBox> detectFaceAlign(@NonNull byte[] nv21, int with, int heigth);


    /**
     * 检测的最小人脸设置
     * @param minSize 40,60,80,120,...
     * @return true,成功,false,失败
     */
    public static native boolean setMinFaceSize(int minSize);


    /**
     * 线程设置/1,2,4,8 默认 根据硬件自动配置
     * @param threadsNumber  线程数目
     * @return true,成功,false,失败
     */
    public static native boolean setThreadsNumber(int threadsNumber);



    /**
     * 初始化 人脸识别引擎
     * @param context
     * @param network
     * @param type
     * @return 0,初始化成功,其他值为ErrorCode
     */
    public static int JLX_FR_InitEngine(Context context, int network, int type) {
        String packageResourcePath = getApkagePath(context);
        int ret = -1;
        try {
            //本地私有 激活函数
//            ret = active(context, SSLPath);
//            if (ret != JlxStatusBox.JLX_FACE_ACTIVE_DONE) {
//                Log.e(TAG, "Init Engine Failed ,Code :" + ret);
//                return ret;
//            }
            ret = initEngine(network, type, packageResourcePath);
            if (ret != JlxTaskBox.JLX_INITFACE_ENGINE_DONE) {
                Log.e(TAG, "Init Engine Failed ,Code :" + ret);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return ret;
    }

    public void JLX_FR_UnInit(){
         JLXunInitEngine();
    }


    public static  ArrayList<String> JLX_FR_Predict(@NonNull byte[] imgData){
        return JLXfacePredict(imgData);
    }

    public static int JLX_FR_Register(@NonNull byte[] imgData, String name){
        return JLXfaceregister(imgData,name);
    }


    public static synchronized native float[] JLXGetFaceFeature(@NonNull byte[] imgData);

    public static synchronized native float  JLXGetTwoFaceSimilar(@NonNull byte[] imgData1,
                                                                 @NonNull byte[] imgData2);

    public static synchronized native float JLXGetTwoFeatureSimilar(@NonNull float[] imgfeatData1,
                                                                    @NonNull float[] imgfeatData2);
    /**
     *识别
     * @param imgData
     * @return
     */
    private static   native ArrayList<String> JLXfacePredict(@NonNull byte[] imgData);

    /**
     *注册
     * @param imgData
     * @param name  人名
     * @return
     */
    private static  native int JLXfaceregister(@NonNull byte[] imgData, String name);


    /**
     * 得到当前已注册用户
     * @return
     */
    public static synchronized native String[] JLXgetUserList();

    /**
     * 删除用户
     * @param name
     * @return
     */
    public static synchronized native int JLXdelUserName(String name);


    /**
     * 人脸检测 初始化
     * @param Modelpath 模型路径
     * @return 0 ,初始化OK,其他失败
     */
    private static native int initdetectModel(@NonNull String Modelpath);

    /**
     * 释放 人脸检测 模型相关內存
     * @return true,成功,false,失败
     */
    private native static boolean releseModel();


    /**
     * 获取apk 安装路径
     * @param context
     * @return
     */
    private static String getApkagePath(Context context) {
        String packageResourcePath = context.getApplicationContext().getCacheDir().getPath();
        if (packageResourcePath.endsWith("/cache")) {
            packageResourcePath = packageResourcePath.substring(0, packageResourcePath.length() - 6);
        }
        return packageResourcePath;
    }


    /**
     * @param network
     * @param type
     * @param packageResourcePath
     * @return
     */
    private static synchronized native int initEngine(int network, int type, String packageResourcePath);


    /**
     * 销毁 人脸识别引擎 内存
     */
    private static native void JLXunInitEngine();

    /**
     *检验
     * @param context 上下文
     * @param sslfilepath 文件路径
     * @return
     */
    private static int active(Context context, String sslfilepath) {
        if (active_status) {
            return JlxStatusBox.JLX_FACE_ACTIVE_DONE;
        }
        String packageResourcePath = getApkagePath(context);

        String DeviceIdstr = DeviceIdUtil.getDeviceId(context);
        if (DeviceIdstr.isEmpty()) {
            return JlxStatusBox.JLX_FACE_ACTIVE_DEVICE_STATUS_ERROR;
        }

        File mfile = new File(sslfilepath);

        if (mfile.exists()) {
            boolean res = JlxEncrypt.check(DeviceIdstr, sslfilepath);
            if (res) {
                //Log.i(TAG,"check OK " + JlxEncrypt.getCode());
                active_status = true;
                return JlxStatusBox.JLX_FACE_ACTIVE_DONE;
            } else {
                return JlxStatusBox.JLX_FACE_ACTIVE_CHECK_ENCRYPT_FAIL;
            }
        } else {
            try {
                //路径 应该跟apk包相关联
                File mf = new File(packageResourcePath + File.separator + "files" + File.separator + deviceInfo);
                if (mf.exists()) {
                    //...读取文件内容 if equal return
                    FileUtil.write(DeviceIdstr, mf);
                } else {
                    mf.createNewFile();
                    FileUtil.write(DeviceIdstr, mf);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            return JlxStatusBox.JLX_FACE_ACTIVE_FILE_EMPTY_FAIL;
        }

    }

}
