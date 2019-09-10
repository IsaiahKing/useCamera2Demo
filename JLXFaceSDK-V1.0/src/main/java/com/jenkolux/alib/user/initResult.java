package com.jenkolux.alib.user;

public interface initResult {
    void     success(Boolean b);
    void     failed(int error);
    boolean  completed();
}
