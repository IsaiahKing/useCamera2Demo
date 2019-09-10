package com.jlx.status;

public class JlxStatusBox {
    /**
     * 注册人脸状态码，准备注册
     */
    public static final int JLX_REGISTER_STATUS_READY = 0;
    /**
     * 注册人脸状态码，注册中
     */
    public static final int JLX_REGISTER_STATUS_PROCESSING = 1;
    /**
     * 注册人脸状态码，注册结束（无论成功失败）
     */
    public static final int JLX_REGISTER_STATUS_DONE = 2;

    public static int       JLX_registerStatus = JLX_REGISTER_STATUS_DONE;

    /**
     *识别准备中
     */
    public static final int JLX_PREDICT_STATUS_PREPARE = 5;

    /**
     *识别中
     */
    public static final int JLX_PREDICT_STATUS_PROCESSING = 7;

    public static volatile int   JLX_PREDICT_STATUS = JLX_PREDICT_STATUS_PREPARE;


    /**
     * 准备 初始化人脸检测
     */
    public static final int JLX_FACE_DETECT_STATUS_READY = 10;


    public static int JLX_FACE_DETECT_STATUS = JLX_FACE_DETECT_STATUS_READY;
    /**
     * 人脸检测完成
     */
    public static final int JLX_FACE_DETECT_STATUS_END = 12;

    /**
     * 人脸检测失败状态码
     */
    public static final int JLX_FACE_DETECT_STATUS_FAIL = 13;

    /**
     *激活 准备中
     */
    public  static final int JLX_FACE_ACTIVE_PREPARE = -1;

    public  static int JLX_FACE_ACTIVE_STATUS = JLX_FACE_ACTIVE_PREPARE;


    /**
     * 完成激活,激活成功
     */
    public static final int JLX_FACE_ACTIVE_DONE = 0;

    public static final int JLX_FACE_ACTIVE_END = 90112;

    /**
     * 获取设备状态异常
     */
    public static final int JLX_FACE_ACTIVE_DEVICE_STATUS_ERROR = 90113;

    /**
     * 当前检验文件为空
     */
    public static final int JLX_FACE_ACTIVE_FILE_EMPTY_FAIL = 90115;

    /**
     * 当前检验错误
     */
    public static final int JLX_FACE_ACTIVE_CHECK_ENCRYPT_FAIL = 90114;

    /**
     * 库里面人脸为空
     */
    public static final int JLX_FACE_DB_NUM_EMPTY = 18;

    /**
     * 人脸数目检验 OK
     */
    public static final int JLX_FACE_DB_NUM_DONE = 19;

}
