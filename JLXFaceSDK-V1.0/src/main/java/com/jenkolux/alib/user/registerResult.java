package com.jenkolux.alib.user;

public interface registerResult {
    void success(String path,byte[] Feature);
    void complete(Boolean b);
    void failed(int error);
}
