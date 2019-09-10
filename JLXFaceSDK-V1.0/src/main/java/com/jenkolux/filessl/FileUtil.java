package com.jenkolux.filessl;

import android.os.Environment;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.RandomAccessFile;

public class FileUtil {
    private static final String TAG = "FileUtil";


    //String printTxtPath = getApplicationContext().getPackageResourcePath() + "/files/" + fileName;

    /**
     *
     * @param strcontent 内容
     * @param filePath  路径
     * @param fileName  名字
     */
    // 将字符串写入到文本文件中
    public static boolean writeTxtToFile(String strcontent,
                                      final String filePath,
                                      final String fileName) {

        boolean result = false;
        //生成文件夹之后，再生成文件，不然会出错
        makeFilePath(filePath, fileName);

        String strFilePath  = filePath + fileName;
        // 每次写入时，都换行写
        String strContent   = strcontent + "\r\n";
        try {
            File file = new File(strFilePath);
            if (!file.exists()) {
                Log.d(TAG, "Create the file:" + strFilePath);
                file.getParentFile().mkdirs();
                result = file.createNewFile();
            }
            RandomAccessFile raf = new RandomAccessFile(file, "rwd");
            raf.seek(file.length());
            raf.write(strContent.getBytes());
            raf.close();
        } catch (Exception e) {
            Log.e(TAG, "Error on write File:" + e);
        }
        return result;
    }


    //生成文件
    public static boolean makeFilePath(final String filepath,
                                       final String fileName){
        File file;
        boolean result;
        if(makeRootDirectory(filepath)){
            try{
                file = new File(filepath + fileName);
                if(!file.exists()){
                    result = file.createNewFile();
                    return result;
                }else{
                    return true;
                }
            }catch (IOException e){
                e.printStackTrace();
            }
        }
        return false;
    }

    //生成文件夹
    public static boolean makeRootDirectory(String filePath){
        File file = null;
        boolean result = false;
        try {
            file = new File(filePath);
            if(!file.exists()){
                result = file.mkdir();
                return result;
            }else{
                return true;
            }
        }catch (Exception e){
            Log.i(TAG,e+"  ");
        }
        return result;
    }

    /**
     * 读取数据
     * @param file
     * @return
     */
    public static String getFileContent(File file) {
        String content = "";
        if (!file.isDirectory()) {  //检查此路径名的文件是否是一个目录(文件夹)
            if (file.getName().endsWith("txt")) {//文件格式为""文件
                try {
                    InputStream instream = new FileInputStream(file);
                    if (instream != null) {
                        InputStreamReader inputreader
                                = new InputStreamReader(instream, "UTF-8");
                        BufferedReader buffreader = new BufferedReader(inputreader);
                        String line = "";
                        //分行读取
                        while ((line = buffreader.readLine()) != null) {
                            content += line + "\n";
                        }
                        instream.close();//关闭输入流
                    }
                } catch (java.io.FileNotFoundException e) {
                    Log.d(TAG, "The File doesn't not exist.");
                } catch (IOException e) {
                    Log.d(TAG, e.getMessage());
                }
            }
        }
        return content;
    }

    /**
     * 写入文件方法
     *
     * @param content
     */
    public static void write(String content, File targetFile) {
        try {
            //判断实际是否有SD卡，且应用程序是否有读写SD卡的能力，有则返回true
            if (Environment.getExternalStorageState().equals(
                    Environment.MEDIA_MOUNTED)) {
                // 获取SD卡的目录

                //使用outputstream则是要先清空内容再写入
                OutputStream raf = new FileOutputStream(targetFile);

                raf.write(content.getBytes());
                raf.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
