package com.jenkolux.util;

public class JlxScreenUtil {
    //显示的宽度
    private  static int PRE_WIDTH;
    //显示的高度
    private  static int PRE_HEIGHT;

    public static int  getPreWidth(){
        return PRE_WIDTH;
    }
    public static int  getPreHeight(){
        return PRE_HEIGHT;
    }

    public static void setPreWidthHeight(int width,int height){
        PRE_WIDTH  = width;
        PRE_HEIGHT = height;
    }

}
