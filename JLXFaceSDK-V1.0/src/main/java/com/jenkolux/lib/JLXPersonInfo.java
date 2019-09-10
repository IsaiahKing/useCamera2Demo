package com.jenkolux.lib;


public class JLXPersonInfo {
    private  String name = null;
    private  int    age = 0;
    private  int    sex;
    private  float  score = 0.0f;

    public JLXPersonInfo(){
    }
    public void setName(String name){
        this.name  = name;
    }
    public void setScore(float score){
        this.score = score;
    }
    public  String getName() {
        return name;
    }
    public  int    getAge(){
        return age;
    }
    public  float  getScore(){
        return score;
    }
}
