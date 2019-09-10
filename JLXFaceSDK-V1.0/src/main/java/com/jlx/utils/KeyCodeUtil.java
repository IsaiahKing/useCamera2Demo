package com.jlx.utils;

import android.annotation.SuppressLint;
import android.media.AudioManager;
import android.media.ToneGenerator;
import android.view.KeyEvent;

import java.util.HashMap;
import java.util.Map;

/**
 * 
 * 按键code转换为String
 */
public class KeyCodeUtil {
	
	@SuppressLint("UseSparseArrays") private static Map<Integer,String> keyCodeMap = new HashMap<Integer, String>();
	
	private static ToneGenerator ton = new ToneGenerator(AudioManager.FX_KEY_CLICK, 40);
	
	static {
		keyCodeMap.put(KeyEvent.KEYCODE_0, "0");
		keyCodeMap.put(KeyEvent.KEYCODE_1, "1");
		keyCodeMap.put(KeyEvent.KEYCODE_2, "2");
		keyCodeMap.put(KeyEvent.KEYCODE_3, "3");
		keyCodeMap.put(KeyEvent.KEYCODE_4, "4");
		keyCodeMap.put(KeyEvent.KEYCODE_5, "5");
		keyCodeMap.put(KeyEvent.KEYCODE_6, "6");
		keyCodeMap.put(KeyEvent.KEYCODE_7, "7");
		keyCodeMap.put(KeyEvent.KEYCODE_8, "8");
		keyCodeMap.put(KeyEvent.KEYCODE_9, "9");
		keyCodeMap.put(KeyEvent.KEYCODE_POUND, "#");
		keyCodeMap.put(KeyEvent.KEYCODE_STAR, "*");
	}
	
	/**
	 * KeyCode转换为String,支持门口机十二键转换
	 * 
	 * @param KeyCode
	 * @return String
	 * */
	public static String keyCodeToInteger(int KeyCode){
		
		if (keyCodeMap.get(Integer.valueOf(KeyCode)) == null) {
			
			return null;
		}
		
		return keyCodeMap.get(Integer.valueOf(KeyCode));
	}
	
	public static void soundOnKeyDown()
	{
		ton.startTone(ToneGenerator.TONE_CDMA_KEYPAD_VOLUME_KEY_LITE);
	}

}
