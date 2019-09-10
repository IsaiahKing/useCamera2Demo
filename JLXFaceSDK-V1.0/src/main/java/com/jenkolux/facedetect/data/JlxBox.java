package com.jenkolux.facedetect.data;

import android.graphics.Rect;

public class JlxBox {
    public JlxBox(int x1,int y1,int x2,int y2){
        _x1 = x1;
        _x2 = x2;
        _y1 = y1;
        _y2 = y2;
    }

    float score;
    int _x1;
    int _y1;
    int _x2;
    int _y2;
    float area;
    float ppoint[];
    float regreCoord[];

    int alignface = 0;
    private static final int face_width = 224;
    private static final int face_height = 224;

    public void setPpoint(){
    }
    public int getAlignface(){
        return alignface;
    }
    byte[] face_final = new byte[face_width*face_height*3/2];

    public byte[] getface(){
        return  face_final;
    }

    public Rect getRect(){
        return new Rect(_x1,_y1,_x2,_y2);
    }
    public void setScore(float f){
        score = f;
    }
}
