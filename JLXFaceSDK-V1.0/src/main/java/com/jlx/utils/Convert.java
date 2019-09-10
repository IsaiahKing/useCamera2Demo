package com.jlx.utils;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;


import com.jenkolux.lib.FaceDet;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

public class Convert {

    /**
     * RGB转BGR （size : 224×224）
     *
     * @param b_g_r
     * @param imgDataRgb
     */
    public static void Rgb2bgr(byte[] b_g_r,int[] imgDataRgb){
        int offset1;
        int offset2;
        int offset3;
        for (int i = 0; i < 224; i++)
        {
            offset1 = 224 * i;
            offset2 = 50176 + offset1;
            offset3 = 100352 + offset1;
            for (int j = 0; j < 224; j++) {//BGR
                b_g_r[offset1 + j] = (byte) ((imgDataRgb[offset1 + j]) & 0xFF);
                b_g_r[offset2 + j] = (byte) ((imgDataRgb[offset1 + j] >> 8) & 0xFF);
                b_g_r[offset3 + j] = (byte) ((imgDataRgb[offset1 + j] >> 16) & 0xFF);
            }
        }
    }

    /**
     * bitmap转YUV
     * @param inputwidth
     * @param inputHeight
     * @param scaled
     * @return
     */
    public static byte[] bitmaptoYuv(int inputwidth,int inputHeight,Bitmap scaled){
        int[] argb = new int[inputHeight*inputwidth];
        scaled.getPixels(argb, 0, inputwidth, 0, 0, inputwidth, inputHeight);

        byte [] yuv = new byte[inputwidth*inputHeight*3/2];
        encodeYUV420SP(yuv,argb, inputwidth, inputHeight);

        scaled.recycle();

        return yuv;
    }

    /***
     *
     * @param nv21 源数据
     * @param width 宽
     * @param height 高
     * @param srcRect 矩形
     * @return （矩形的bitmap）
     */
    public static Bitmap nv21ToScaledBitmap(byte[] nv21, int width, int height,Rect srcRect) {
        Bitmap bitmap = null;
        try {
            YuvImage image = new YuvImage(nv21, ImageFormat.NV21, width, height, null);
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            image.compressToJpeg(new Rect(srcRect.left, srcRect.top, srcRect.right, srcRect.bottom), 100, stream);

            bitmap = BitmapFactory.decodeByteArray(stream.toByteArray(), 0, stream.size());
            stream.close();
        }
        catch (IOException e) { e.printStackTrace();
        }
        return bitmap;
    }

    /**
     * YUV420SP解码
     * @param rgb
     * @param yuv420sp
     * @param width
     * @param height
     * @param brightness_factor
     */
    private static void decodeYUV420SP(int[] rgb, byte[] yuv420sp, int width, int height, int brightness_factor) {
        final int frameSize = width * height;

        brightness_factor = 1192 * brightness_factor / 1000;

        for (int j = 0, yp = 0; j < height; j++) {
            int uvp = frameSize + (j >> 1) * width, u = 0, v = 0;
            for (int i = 0; i < width; i++, yp++) {
                int y = (0xff & ((int) yuv420sp[yp])) - 16;
                if (y < 0)
                    y = 0;
                if ((i & 1) == 0) {
                    v = (0xff & yuv420sp[uvp++]) - 128;
                    u = (0xff & yuv420sp[uvp++]) - 128;
                }

                int y1192 = brightness_factor * y; //1192 * y * brightness_factor /1000;
                int r = (y1192 + 1634 * v);
                int g = (y1192 - 833 * v - 400 * u);
                int b = (y1192 + 2066 * u);

                if (r < 0)
                    r = 0;
                else if (r > 262143)
                    r = 262143;
                if (g < 0)
                    g = 0;
                else if (g > 262143)
                    g = 262143;
                if (b < 0)
                    b = 0;
                else if (b > 262143)
                    b = 262143;

                rgb[yp] = 0xff000000 | ((r << 6) & 0xff0000) | ((g >> 2) & 0xff00) | ((b >> 10) & 0xff);
            }
        }
    }


    public static byte[] getPixelsBGR(Bitmap image) {
        // calculate how many bytes our image consists of
        int bytes = image.getByteCount();

        ByteBuffer buffer = ByteBuffer.allocate(bytes); // Create a new buffer
        image.copyPixelsToBuffer(buffer); // Move the byte data to the buffer

        byte[] temp = buffer.array(); // Get the underlying array containing the data.

        byte[] pixels = new byte[(temp.length/4) * 3]; // Allocate for BGR

        // Copy pixels into place
        for (int i = 0; i < temp.length/4; i++) {

            pixels[i * 3] = temp[i * 4 + 2];        //B
            pixels[i * 3 + 1] = temp[i * 4 + 1];    //G
            pixels[i * 3 + 2] = temp[i * 4 ];       //R
        }

        return pixels;
    }

    //NV21: YYYY VUVU

    /**
     * nv21 数据镜像
     * @param nv21_data
     * @param width
     * @param height
     * @return
     */
    public static byte[] NV21_mirror(byte[] nv21_data, int width, int height)
    {
         int i;
         int left, right;
         byte temp;
         int startPos = 0;
         // mirror Y
        for (i = 0; i < height; i++)
        {
            left = startPos;
            right = startPos + width - 1;
            while (left < right)
            {
                temp= nv21_data[left];
                nv21_data[left] = nv21_data[right];
                nv21_data[right] = temp;
                left ++;
                right--;
            }
            startPos += width;
        }

        // mirror U and V
        int offset = width * height;
        startPos = 0;
        for (i = 0; i < height / 2; i++)
        {
            left = offset + startPos;
            right = offset + startPos + width - 2;
            while (left < right)
            {
                temp= nv21_data[left ];
                nv21_data[left ] = nv21_data[right];
                nv21_data[right] = temp;
                left ++;
                right--;

                temp= nv21_data[left ];
                nv21_data[left ] = nv21_data[right];
                nv21_data[right] = temp;
                left ++;
                right--;
            }
            startPos += width;
        }
        return nv21_data;
    }

    /**
     * 功能：  旋转270度 并且镜像
     * @param data
     * @param imageWidth
     * @param imageHeight
     * @return
     */
    public static byte[] rotateYUVDegree270AndMirror(byte[] data, int imageWidth, int imageHeight) {
        byte[] yuv = new byte[imageWidth * imageHeight * 3 / 2];
        // Rotate and mirror the Y luma
        int i = 0;
        int maxY = 0;
        for (int x = imageWidth - 1; x >= 0; x--) {
            maxY = imageWidth * (imageHeight - 1) + x * 2;
            for (int y = 0; y < imageHeight; y++) {
                yuv[i] = data[maxY - (y * imageWidth + x)];
                i++;
            }
        }
        // Rotate and mirror the U and V color components
        int uvSize = imageWidth * imageHeight;
        i = uvSize;
        int maxUV = 0;
        for (int x = imageWidth - 1; x > 0; x = x - 2) {
            maxUV = imageWidth * (imageHeight / 2 - 1) + x * 2 + uvSize;
            for (int y = 0; y < imageHeight / 2; y++) {
                yuv[i] = data[maxUV - 2 - (y * imageWidth + x - 1)];
                i++;
                yuv[i] = data[maxUV - (y * imageWidth + x)];
                i++;
            }
        }
        return yuv;
    }

    /**
     * ARGB转nv21
     * @param argb
     * @param width
     * @param height
     * @return
     */
    public static byte[] argbToNv21(int[] argb, int width, int height) {
        int frameSize = width * height;
        int yIndex = 0;
        int uvIndex = frameSize;
        int index = 0;
        byte[] nv21 = new byte[width * height * 3 / 2];
        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                int R = (argb[index] & 0xFF0000) >> 16;
                int G = (argb[index] & 0x00FF00) >> 8;
                int B = argb[index] & 0x0000FF;
                int Y = (66 * R + 129 * G + 25 * B + 128 >> 8) + 16;
                int U = (-38 * R - 74 * G + 112 * B + 128 >> 8) + 128;
                int V = (112 * R - 94 * G - 18 * B + 128 >> 8) + 128;
                nv21[yIndex++] = (byte) (Y < 0 ? 0 : (Y > 255 ? 255 :
                        Y));
                if (j % 2 == 0 && index % 2 == 0 && uvIndex < nv21.length
                        - 2) {
                    nv21[uvIndex++] = (byte) (V < 0 ? 0 : (V > 255 ?255 : V));
                    nv21[uvIndex++] = (byte) (U < 0 ? 0 : (U > 255 ?
                            255 : U));
                }
                ++index;
            }
        }
        return nv21;
    }

    /**
     * YuvSP转RGB
     * @param data
     * @param width
     * @param height
     * @return
     */
    public  static  native int[] yuv420sp2rgb(byte[] data,int width, int height);

    /**
     * YUV420SP编码
     * @param yuv420sp
     * @param argb
     * @param width
     * @param height
     */
    private static void encodeYUV420SP(byte[] yuv420sp, int[] argb, int width, int height) {
        final int frameSize = width * height;

        int yIndex = 0;
        int uvIndex = frameSize;

        int a, R, G, B, Y, U, V;
        int index = 0;
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {

                a = (argb[index] & 0xff000000) >> 24; // a is not used obviously
                R = (argb[index] & 0xff0000) >> 16;
                G = (argb[index] & 0xff00) >> 8;
                B = (argb[index] & 0xff);

                // well known RGB to YUV algorithm
                Y = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16;
                U = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
                V = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128;

                // NV21 has a plane of Y and interleaved planes of VU each sampled by a factor of 2
                //    meaning for every 4 Y pixels there are 1 V and 1 U.  Note the sampling is every other
                //    pixel AND every other scanline.
                yuv420sp[yIndex++] = (byte) ((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
                if (j % 2 == 0 && index % 2 == 0) {
                    yuv420sp[uvIndex++] = (byte) ((V < 0) ? 0 : ((V > 255) ? 255 : V));
                    yuv420sp[uvIndex++] = (byte) ((U < 0) ? 0 : ((U > 255) ? 255 : U));
                }

                index ++;
            }
        }
    }
}
