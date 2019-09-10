package com.jlx.helps;

import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.support.annotation.NonNull;
import android.util.Log;

import com.jenkolux.facedetect.data.JlxBox;

import com.jenkolux.lib.JLXRect;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.jenkolux.util.JlxScreenUtil;
import com.jlx.utils.Convert;


/**
 * Created At 2018/5/11 9:49.
 *
 * @author larry
 */

public class FaceHelper {
    private static final   String TAG = "FaceHelper";
    static  final  int     RATIO = 1;//处理图片时压缩的比例
    /**
     * excute face methods in one thread
     */

    public static boolean use_front_camera = false;//是否已经启用前置摄像头

    private FaceHelper(){

    }
    private static class FaceHelperHolder{
        static FaceHelper instance = new FaceHelper();
    }
    public  static FaceHelper getInstance() {
        return FaceHelperHolder.instance;
    }
    public  int     position;
    //int[] rgbaImage           = new int[JlxScreenUtil.getPreWidth()* JlxScreenUtil.getPreHeight()];

    final int[] imgDataRgb    = new int[224 * 224];

    /**
     * 找到 人脸中 最大的一个
     * @param faceout
     * @return
     */
    public  int findMaxAreaRect(List<JLXRect> faceout) {
        position = 0;
        int area = 0;
        Rect tempRect;
        int size = faceout.size();
        if (size == 1) {
            return 0;
        }
        for (int i = 0; i < size; i++) {
            tempRect = faceout.get(i).getRect();
            if ((tempRect.right - tempRect.left) * (tempRect.bottom - tempRect.top) > area) {
                position = i;
            }
        }
        return position;
    }

    /***
     * 功能：用于检验框体
     * @param rect
     * @param width
     * @param height
     */
    private void checkRect(Rect rect,int width,int height){
        if(rect.left < 0){
            rect.left = 0;
        }
        if (rect.top < 0){
            rect.top = 0;
        }
        if(rect.bottom >= height){
            rect.bottom = height;
        }
        if (rect.right >= width){
            rect.right = width;
        }
    }

    /**
    * 保存图片
    *
     * @param bmp 输入图像
     * @param picName 名字jpg后缀
    * */

    public void saveBitmap(Bitmap bmp,String picName) {
        Log.e(TAG, "保存图片");
        File f = new File("/mnt/sdcard/", picName);
        if (f.exists()) {
            f.delete();
        }
        try {
            FileOutputStream out = new FileOutputStream(f);
            bmp.compress(Bitmap.CompressFormat.PNG, 90, out);
            out.flush();
            out.close();

        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    /**
     *  数据格式转化
     * @param faceInfoList
     * @return  （JLXRect数据 结构 可用于显示）
     */

    @SuppressWarnings("unchecked")
    public  List<JLXRect>  scaleRect(@NonNull List<JlxBox>  faceInfoList,int SCREEN_WIDTH,int SCREEN_HEIGHT){
        List<JLXRect>   jlxRects  = new ArrayList<JLXRect>();
        if(faceInfoList.size() > 0)
        {
            for (JlxBox t : faceInfoList) {
                //Log.d("arcsoft", "Face name :" + face.toString());
                JLXRect rect = new JLXRect(new Rect(t.getRect()));
                //缩放人脸框
                rect.zoom((float) FaceHelper.RATIO * SCREEN_WIDTH / JlxScreenUtil.getPreWidth(),
                        (float) FaceHelper.RATIO * SCREEN_HEIGHT / JlxScreenUtil.getPreHeight());
                jlxRects.add(rect);
            }
        }
        return jlxRects;
    }

    /***
     * 得到 最合适的框
     * @param width
     * @param height
     * @param srcRect
     * @return
     */
    public static Rect getBestRect(int width, int height, Rect srcRect) {
        if (srcRect == null) {
            return null;
        } else {
            Rect rect = new Rect(srcRect);
            int maxOverFlow = 0;

            if (rect.left < 0) {
                maxOverFlow = -rect.left;
            }

            int tempOverFlow;
            if (rect.top < 0) {
                tempOverFlow = -rect.top;
                if (tempOverFlow > maxOverFlow) {
                    maxOverFlow = tempOverFlow;
                }
            }

            if (rect.right > width) {
                tempOverFlow = rect.right - width;
                if (tempOverFlow > maxOverFlow) {
                    maxOverFlow = tempOverFlow;
                }
            }

            if (rect.bottom > height) {
                tempOverFlow = rect.bottom - height;
                if (tempOverFlow > maxOverFlow) {
                    maxOverFlow = tempOverFlow;
                }
            }

            if (maxOverFlow != 0) {
                rect.left += maxOverFlow;
                rect.top += maxOverFlow;
                rect.right -= maxOverFlow;
                rect.bottom -= maxOverFlow;
                return rect;
            } else {
                int padding = rect.height() / 2;
                if (rect.left - padding <= 0 || rect.right + padding >= width || rect.top - padding <= 0 || rect.bottom + padding >= height) {
                    padding = Math.min(Math.min(Math.min(rect.left, width - rect.right), height - rect.bottom), rect.top);
                }

                rect.left -= padding;
                rect.top -= padding;
                rect.right += padding;
                rect.bottom += padding;
                return rect;
            }
        }
    }


    /***
     *
     * @param nv21 源数据
     * @param width  宽
     * @param height   高
     * @param faceInfoList 人脸信息
     * @return （返回224×224的 bgr数据）
     */
//    public byte[] Facedata2bgr(byte[] nv21,int width,int height,List<FaceInfo> faceInfoList){
//        Rect    facetmp    = getBestRect(width,height,(faceInfoList.get(0)).getRect());
//
//        Bitmap  bitmap_1   = DetectAnsyTask.nv21_bitmap(nv21,width,height);
//
//        //截取face
//        Bitmap  bitmap_2 = Bitmap.createBitmap(bitmap_1,facetmp.left,facetmp.top,facetmp.width(),facetmp.height());
//
//        //压缩 到 224*224
//        Bitmap  bitmap_3   = Bitmap.createScaledBitmap(bitmap_2,224,224,false);
//
//        bitmap_3.getPixels(imgDataRgb, 0, 224, 0, 0, 224, 224);
//
//        ImageUtils.recycleBitmap(bitmap_1);
//        ImageUtils.recycleBitmap(bitmap_2);
//
//        byte[] b_g_r = new byte[224 * 224 * 3];
//        Convert.Rgb2bgr(b_g_r,imgDataRgb);
//
//        ImageUtils.recycleBitmap(bitmap_3);
//        return b_g_r;
//    }

    public byte[] Facedata2bgr(byte[] nv21,int width,int height,ArrayList<JlxBox> faceInfoList){
//        Rect    facetmp    = getBestRect(width,height,(faceInfoList.get(0)).getRect());
//
//        Bitmap  bitmap_1   = DetectAnsyTask.nv21_bitmap(nv21,width,height);
//
//        //截取face
//        Bitmap  bitmap_2 = Bitmap.createBitmap(bitmap_1,facetmp.left,facetmp.top,facetmp.width(),facetmp.height());
//
//        //压缩 到 224*224
//        Bitmap  bitmap_3   = Bitmap.createScaledBitmap(bitmap_2,224,224,false);
//
//        bitmap_3.getPixels(imgDataRgb, 0, 224, 0, 0, 224, 224);
//
//        ImageUtils.recycleBitmap(bitmap_1);
//        ImageUtils.recycleBitmap(bitmap_2);

        byte[] b_g_r = new byte[224 * 224 * 3];
        Convert.Rgb2bgr(b_g_r,imgDataRgb);

        //ImageUtils.recycleBitmap(bitmap_3);
        return b_g_r;
    }
}
