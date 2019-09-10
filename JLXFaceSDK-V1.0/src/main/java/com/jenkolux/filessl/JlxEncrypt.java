package com.jenkolux.filessl;


import android.util.Log;

/**
 * 使用c/c++　方式加解密
 */
public class JlxEncrypt {
    private static final String TAG = "JlxEncrypt";
    private static String getCodeStr = null;

    static {
        try{
            System.loadLibrary("jlx_filessl");
            Log.i(TAG, "Load jlx_filessl library succeed");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "jlx_filessl library not found");
        }
    }
    private native static  String encode(String filepath);

    public  final  static  String desFileName = "license";

    public  static boolean check(String s1,String filepath){
        boolean result = false;
        //如果没有校验
        getCodeStr = encode(filepath);
        if(getCodeStr.equals(s1)){
            result = true;
        }
        return  result;
    }

    public static String getCode(){
        return getCodeStr;
    }

}
