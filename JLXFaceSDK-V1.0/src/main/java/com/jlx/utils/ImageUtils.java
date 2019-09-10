package com.jlx.utils;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.UUID;

/**
 * Created At 2018/5/8 15:07.
 *
 * @author larry
 */

public class ImageUtils {

//    static {
//        System.loadLibrary("ImageUtils");
//    }

    private ImageUtils() {
        // Forbidden to initialize.
    }

    /**
     * 尺寸压缩（通过缩放图片像素来减少图片占用内存大小）
     *
     * @param bmp 原始图片
     * @param ratio 尺寸压缩倍数,值越大，图片尺寸越小
     */

    public static Bitmap sizeCompress(Bitmap bmp, int ratio) {
        // 压缩Bitmap到对应尺寸
        Bitmap result = Bitmap.createBitmap(bmp.getWidth() / ratio, bmp.getHeight() / ratio, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(result);
        Rect rect = new Rect(0, 0, bmp.getWidth() / ratio, bmp.getHeight() / ratio);
        canvas.drawBitmap(bmp, null, rect, null);
        bmp.recycle();
        return result;
    }
//
//    @Keep
//    public native int[] yuv420sp2rgb(byte[] data,int width, int height);



    /**
     * 保存Bitmap至本地
     *
     * @param bmp    照片数据
     */
    public static void saveBitmapToFile(Context context, String verifyValue, Bitmap bmp) {
        String filePath = getFaceDetectFilePath();
        String fileName = getFileName(verifyValue);
        String fileAbsolutePath = filePath + File.separator + fileName;
        File file = new File(fileAbsolutePath);
        FileOutputStream fOut;
        try {
            fOut = new FileOutputStream(file);
            bmp.compress(Bitmap.CompressFormat.JPEG, 85, fOut);
            fOut.flush();
            fOut.close();
            Log.e("ImageUtils", "保存图片 = "+fileAbsolutePath);
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
//            recycleBitmap(bmp);
        }
    }
    public static String getFaceDetectFilePath(){
        String proFileName = Environment.getExternalStorageDirectory().getAbsolutePath()
                + File.separator + "JLXPic"
                + File.separator + "FaceDetectPic";
        File file = new File(proFileName);
        if (!file.exists()) {
            file.mkdirs();
        }
        return proFileName;
    }


    /**
     * 抓拍图片存储路径
     * @param msg 事件值
     * @return 文件名
     */
    public static String getFileName(String msg) {
        String fileName = null;
        SimpleDateFormat sdf=new SimpleDateFormat("yyyyMMddHHmmss", Locale.getDefault());

        fileName = sdf.format(new Date()) + "-" + msg + ".JPG";
        return fileName;
    }

    /**
     * 回收Bitmap，防止出现oom
     * @param bm bitmap
     */
    public static void recycleBitmap(Bitmap bm){
        if (bm != null && !bm.isRecycled()) {
            bm.recycle();
            System.gc();
        }
    }


    //保存nv21数据　图片用例-------------------------------------
    public static void bytesToImageFile(byte[] bytes) {
        try {
            File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + UUID.randomUUID().toString() + ".yuv");
            FileOutputStream fos = new FileOutputStream(file);
            fos.write(bytes, 0, bytes.length);
            fos.flush();
            fos.close();
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }
}
