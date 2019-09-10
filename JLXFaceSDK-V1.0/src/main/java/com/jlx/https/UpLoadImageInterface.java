package com.jlx.https;

import io.reactivex.Observable;
import okhttp3.MultipartBody;
import okhttp3.RequestBody;
import okhttp3.ResponseBody;
import retrofit2.http.Multipart;
import retrofit2.http.POST;
import retrofit2.http.Part;

public interface UpLoadImageInterface {
    @Multipart
    @POST("/upload")
    Observable<ResponseBody> upLoadImage(@Part MultipartBody.Part part);
}
