package com.jlx.https;

import android.content.Context;
import android.util.Log;

import com.jlx.https.RetrofitServiceManager;
import com.jlx.https.UpLoadImageInterface;

import java.io.File;

import io.reactivex.Observable;
import io.reactivex.Observer;
import io.reactivex.Scheduler;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.schedulers.Schedulers;
import okhttp3.MediaType;
import okhttp3.MultipartBody;
import okhttp3.RequestBody;

import okhttp3.ResponseBody;

public class FaceUpLoader {
    private UpLoadImageInterface mUploadFace;
    private static final String TAG = "FaceUpLoader";
    private FaceUpLoader(Context context){
        mContext = context;
        mUploadFace = RetrofitServiceManager.getInstance().create(UpLoadImageInterface.class);
    }
    private static Context mContext;
    private static FaceUpLoader mInstance;
    public  void up(String path){
        File file = new File(path);
        if(file.exists()){
            final MultipartBody.Builder builder = new MultipartBody.Builder();
            final RequestBody       requestBody = RequestBody.create(MediaType.parse("multipart/form-data"),file);
            MultipartBody.Part body = MultipartBody.Part.createFormData("image",file.getName(),requestBody);

            mUploadFace.upLoadImage(body).subscribeOn(Schedulers.io())
                    .observeOn(Schedulers.newThread())
                    .subscribe(new Observer<ResponseBody>() {
                        @Override
                        public void onSubscribe(Disposable d) {

                        }

                        @Override
                        public void onNext(ResponseBody responseBody) {
                            Log.e(TAG,"next : "+ requestBody.toString());
                        }

                        @Override
                        public void onError(Throwable e) {
                            Log.e(TAG,"up load error :" + e.getMessage());
                        }

                        @Override
                        public void onComplete() {

                        }
                    });
        }
    }

    private UpLoadImageInterface getinterface(){
        return mUploadFace;
    }

    public static FaceUpLoader getInstance(Context context){
        if(context != null){
            mContext = context;
        }
        synchronized (FaceUpLoader.class){
                if(mInstance == null)
                    mInstance = new FaceUpLoader(context);
        }
        return mInstance;
    }
}
