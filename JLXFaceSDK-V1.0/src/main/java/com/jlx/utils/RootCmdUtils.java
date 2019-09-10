package com.jlx.utils;

import android.util.Log;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.Vector;

public final class RootCmdUtils {
	private static final String TAG = "RootCmdUtils";
	/**
	 * 判断手机是否有root权限
	 */
	public static boolean requestGtiUSBPerssion(){
		if (!hasRootPerssion()) {
			Log.e(TAG, "  No root permission");
		}

		PrintWriter PrintWriter = null;
		Process process = null;
		try {
			process = Runtime.getRuntime().exec("su");
			PrintWriter = new PrintWriter(process.getOutputStream());
			PrintWriter.println("chmod -R 777 /dev/usb ");// /dev/sg*   /dev/block/mmcblk1
//          PrintWriter.println("exit");
			PrintWriter.flush();
			PrintWriter.close();
			int value = process.waitFor();
			return returnResult(value);
		} catch (Exception e) {
			e.printStackTrace();
		}finally{
			if(process!=null){
				process.destroy();
			}
		}
		return false;
	}

	public static boolean requestGtiPerssion(String path){
		if (!hasRootPerssion()) {
			Log.e(TAG, "No root permission");
			return false;
		}
		//判断文件是否存
		try{
			File file = new File(path);
			if(file.exists()){
				PrintWriter PrintWriter = null;
				Process process = null;
				try {
					process = Runtime.getRuntime().exec("su");
					PrintWriter = new PrintWriter(process.getOutputStream());
					String exe_cmd = "sh " + path;
					PrintWriter.println(exe_cmd);// /dev/sg*
//          		PrintWriter.println("exit");
					PrintWriter.flush();
					PrintWriter.close();
					int value = process.waitFor();
					return returnResult(value);
				} catch (Exception e) {
					e.printStackTrace();
				}finally{
					if(process!=null){
						process.destroy();
					}
				}
			}else{
				Log.e(TAG,"read permission file error");
				return  false;
			}
		}catch (Exception e){
			e.printStackTrace();
		}
		return false;
	}


	private static boolean returnResult(int value){
		// 代表成功
		if (value == 0) {
			Log.e("RootCmdUtils", "has  root permissions");
			return true;
		} else if (value == 1) { // 失败
			Log.e("RootCmdUtils", "No root permissions");
			return false;
		} else { // 未知情况
			Log.e("RootCmdUtils", "has no root permissions");
			return false;
		}

	}


	/**
	 * 执行linux命令并且输出结果
	 * @param paramString 命令
	 * @return 结果
	 */
	public static Vector execRootCmd(String paramString) {
		Vector localVector = new Vector();
		try {
			Process localProcess = Runtime.getRuntime().exec("su ");// 经过Root处理的android系统即有su命令
			OutputStream localOutputStream = localProcess.getOutputStream();
			DataOutputStream localDataOutputStream = new DataOutputStream(
					localOutputStream);
			InputStream localInputStream = localProcess.getInputStream();
			DataInputStream localDataInputStream = new DataInputStream(
					localInputStream);
			String str1 = String.valueOf(paramString);
			String str2 = str1 + "\n";
			localDataOutputStream.writeBytes(str2);
			localDataOutputStream.flush();
			String str3 = localDataInputStream.readLine();
			Log.v("execRootCmd", str3);
			localVector.add(str3);
			localDataOutputStream.writeBytes("exit\n");
			localDataOutputStream.flush();
			localProcess.waitFor();
			return localVector;
		} catch (Exception localException) {
			localException.printStackTrace();
		}
		return localVector;
	}

	/**
	 * 执行linux命令但不关注结果输出
	 * @param paramString 命令
	 * @return
	 */
	public static int execRootCmdSilent(String paramString) {
		try {
			Process localProcess = Runtime.getRuntime().exec("su");
			Object localObject = localProcess.getOutputStream();
			DataOutputStream localDataOutputStream = new DataOutputStream(
					(OutputStream) localObject);
			String str = String.valueOf(paramString);
			localObject = str + "\n";
			localDataOutputStream.writeBytes((String) localObject);
			localDataOutputStream.flush();
			localDataOutputStream.writeBytes("exit\n");
			localDataOutputStream.flush();
			localProcess.waitFor();
			localObject = localProcess.exitValue();
			return 0;
		} catch (Exception localException) {
			localException.printStackTrace();
		}
		return 0;
	}

	/**
	 * 判断是否有root权限
	 */
	public static boolean hasRootPerssion(){
		PrintWriter printWriter = null;
		Process process = null;
		try {
			process = Runtime.getRuntime().exec("su");
			printWriter = new PrintWriter(process.getOutputStream());
			printWriter.flush();
			printWriter.close();
			int value = process.waitFor();
			return returnResult(value);
		} catch (Exception e) {
			Log.e(TAG,"hasRootPerssion  error : " + e.getMessage());
		}finally{
			if(process!=null){
				process.destroy();
			}
		}
		return false;
	}
}
