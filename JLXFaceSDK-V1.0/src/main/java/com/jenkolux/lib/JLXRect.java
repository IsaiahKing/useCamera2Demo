package com.jenkolux.lib;

import android.graphics.Point;
import android.graphics.Rect;

import java.util.ArrayList;

/**
 * Created At 2018/5/16 17:52.
 *
 * @author larry
 */

public class JLXRect {
    private String  mLabel = null;
    private int     index = 0;
    private float   score = 0.0f;
    private Rect    mRect;

    private ArrayList<Point> mLandmarkPoints = new ArrayList<>();//add

    public JLXRect(JLXRect self) {
        this.mRect = new Rect(self.getRect());
        this.mLabel = self.getmLabel();
        this.score = self.getScore();
        this.index = self.getIndex();
    }
    public JLXRect(Rect mRect) {
        this.mRect = mRect;
    }

    /**
     * 宽高等比缩放rect
     * @param ratio 缩放因子
     */
    public void zoom(int ratio) {
        mRect.left *= ratio;
        mRect.top *= ratio;
        mRect.right *= ratio;
        mRect.bottom *= ratio;
    }

    /**
     * 宽高不等比缩放rect
     * @param ratioWitdh 缩放因子
     * @param ratioHeight 缩放因子
     */
    public void zoom(float ratioWitdh,float ratioHeight) {
        mRect.left *= ratioWitdh;
        mRect.top *= ratioHeight;
        mRect.right *= ratioWitdh;
        mRect.bottom *= ratioHeight;
    }
    public ArrayList<Point> getFaceLandmarks() {
        return mLandmarkPoints;
    }

    public void setmLandmarkPoints(ArrayList<Point> mLandmarkPoints) {
        this.mLandmarkPoints = mLandmarkPoints;
    }

    public Rect  getRect() {
        return mRect;
    }

    public int   getIndex() {
        return index;
    }

    public float getScore() {
        return score;
    }

    public void    setmRect(Rect mRect) {
        this.mRect = mRect;
    }

    public String  getmLabel() {
        return mLabel;
    }

    public void    setmLabel(String mLabel) {
        this.mLabel = mLabel;
    }

    public void    setIndex(int index) {
        this.index = index;
    }

    public void    setScore(float score) { this.score = score;
    }

    public JLXRect clone() {
        return new JLXRect(this);
    }

}
