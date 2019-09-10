package com.jlx.https;

import com.jlx.https.HttpCommonInterceptor;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

import okhttp3.Interceptor;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import retrofit2.Retrofit;

import retrofit2.adapter.rxjava2.RxJava2CallAdapterFactory;
import retrofit2.converter.gson.GsonConverterFactory;

public class RetrofitServiceManager {
    private static final String TAG = "RetrofitServiceManager";
    private static final int DEFAULT_TIME_OUT = 5;//超时时间 5s
    private static final int DEFAULT_READ_TIME_OUT = 10;
    private static final int DEFAULT_WRITE_TIME_OUT = 10;
    private Retrofit mRetrofit;
    private OkHttpClient.Builder okbuilder;

    public static final String BASE_URL = "http://192.168.5.114:3000/";
    private RetrofitServiceManager() {

        //通过OkHttpClient 可以配置很多东西.比如 链接超时时间,缓存,拦截器 等等
        okbuilder = new OkHttpClient.Builder();
        okbuilder.connectTimeout(DEFAULT_TIME_OUT,TimeUnit.SECONDS);
        okbuilder.writeTimeout(DEFAULT_WRITE_TIME_OUT,TimeUnit.SECONDS);
        okbuilder.readTimeout(DEFAULT_READ_TIME_OUT,TimeUnit.SECONDS);
        okbuilder.retryOnConnectionFailure(true);//错误重连

        //添加公共参数拦截器
        HttpCommonInterceptor commonInterceptor = new HttpCommonInterceptor.Builder()
                .build();

        okbuilder.addInterceptor(commonInterceptor);

        //创建Retrofit
        mRetrofit = new Retrofit.Builder()
                .client(okbuilder.build())
                .addConverterFactory(GsonConverterFactory.create())
                .addCallAdapterFactory(RxJava2CallAdapterFactory.create())
                .baseUrl(BASE_URL)
                .build();
    }

    private static class SingletonHolder{
        private static final RetrofitServiceManager INSTANCE = new RetrofitServiceManager();
    }

    public static RetrofitServiceManager getInstance(){
        return SingletonHolder.INSTANCE;
    }

    public <T> T create(Class<T> service){
        return mRetrofit.create(service);
    }

    private void changeBaseUrl(String url){
        mRetrofit = new Retrofit.Builder()
                .client(okbuilder.build())
                .addConverterFactory(GsonConverterFactory.create())
                .addCallAdapterFactory(RxJava2CallAdapterFactory.create())
                .baseUrl(url)
                .build();

    }

}
