package com.jlx.status;

public class JlxErrorBox {
    /**
     * 无json 配置文件
     */
    public static final int JLX_PARSEJSONFILE_ERROR = 10001;

    /**
     * 无法读取用户相关文件
     */
    public static final int JLX_PARSEGTIFILE_ERROR = 10002;

    public static final int JLX_PARSEFILE_ERROR = 10003;

    /**
     * 初始化时，创建设备 出错
     */
    public static final int JLX_CREATE_DEVICE_ERROR = 10004;

    /**
     * 设备节点或设备名字为空
     */
    public static final int JLX_DEVICE_NODE_EMPRY = 10005;

    /**
     * 初始化 设备节点失败
     */
    public static final int JLX_INIT_DEVICE_NODE_ERROR = 10006;

    /**
     * 当前设备 不支持此类初始化
     */
    public static final int JLX_INIT_DEVICE_NO_SUPPORT_ERROR = 10020;

    /**
     * 当前 分类器创建 错误
     */
    public static final int JLX_INIT_DEVICE_FC_CREATE_ERROR = 10030;


    /**
     * 读取FC 相关文件 错误
     */
    public static final int JLX_READ_FC_FILE_ERROR = 10031;



    /**
     * 识别模型 初始化失败
     */
    public static final int JLX_PREDICT_MODEL_ERROR = -1;
    /**
     * 识别模型 初始化失败 错误码
     */
    public static  volatile int JLX_PREDICT_MODEL_ERROR_CODE = JLX_PREDICT_MODEL_ERROR;

}
