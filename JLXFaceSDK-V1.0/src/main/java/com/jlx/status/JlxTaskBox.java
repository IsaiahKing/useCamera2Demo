package com.jlx.status;

public class JlxTaskBox {

    /**
     * 准备状态,未完成...
     */
    public static final int JLX_THREADPOOL_TASK_PARPARE = 98;

    public static int JLX_THREADPOOL_TASK_STATUS = JLX_THREADPOOL_TASK_PARPARE;
    /**
     * 线程池 任务开启完成
     */
    public static final int JLX_THREADPOOL_TASK_DONE = 99;

    /**
     * 完成人脸识别 模型初始化
     */
    public static final int JLX_INITFACE_ENGINE_DONE = 0;

    /**
     * 完成人脸检测 模型初始化
     */
    public static final int JLX_INITFACE_DETECT_MODEL_DONE = 0;
}
