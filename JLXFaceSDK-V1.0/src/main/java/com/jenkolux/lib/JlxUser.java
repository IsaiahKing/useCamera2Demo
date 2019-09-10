package com.jenkolux.lib;

public class JlxUser {
    public String   name;
    public int      sex;
    public int      age;
    //那个控件
    public int      Tv;

    public static final int TextViewTest = 0;
    public static final int TextViewTip = 1;

    public final String hi = "Hello, ";

    public JlxUser(){
    }

    public String data;

    public  JlxUser(String name,boolean register){
        if(register){
            data = name;
        }
    }
    public JlxUser(String name) {
        this.name = name;
    }
    public JlxUser(String name, int sex) {
        this.name = name;
        this.sex = sex;
    }
    public JlxUser(String name,int age, int sex) {
        this.name = name;
        this.sex = sex;
        this.age = age;
    }

    /***
     * 只显示数据
     * @return
     */
    @Override
    public String toString() {
        return  data;
    }

    public String showAll(){
        return name + sex + age;
    }
}
