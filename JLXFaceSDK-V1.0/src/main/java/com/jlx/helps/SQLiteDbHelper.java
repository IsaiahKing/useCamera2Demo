package com.jlx.helps;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;


import java.util.List;

public class SQLiteDbHelper extends SQLiteOpenHelper {
    public static final String DB_NAME = "ArcFace.db";

    public static final String TABLE_ArcReg = "arcReg";

    public static final String TABLE_History = "arcRegHistory";

    public static final int DB_VERSION = 1;

    private SQLiteDatabase db;

    private static SQLiteDbHelper instance = null;

    //创建 arcReg 表的 sql 语句
    private static final String ArcFaceRegister_CREATE_TABLE_SQL = "create table IF NOT EXISTS " + TABLE_ArcReg + "("
            + "id integer primary key autoincrement,"
            + "name text not null,"
            + "path text not null,"
            + "feature BLOB not null"
            + ");";

    private static final String ArcFaceRegisterHistory_CREATE_TABLE_SQL = "create table IF NOT EXISTS " + TABLE_History + "("
            + "id integer primary key autoincrement,"
            + "name text not null"
            + ");";

    @Override
    public void onCreate(SQLiteDatabase db)
    {
        // 在这里通过 db.execSQL 函数执行 SQL 语句创建所需要的表
        // 创建  表
        this.db = db;
        this.db.execSQL(ArcFaceRegister_CREATE_TABLE_SQL);
        this.db.execSQL(ArcFaceRegisterHistory_CREATE_TABLE_SQL);
    }

    //onCreate 函数会在第一次创建数据库时被调用，因此，在这里执行创建 arcReg 数据库的 SQL 语句.
    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        // 数据库版本号变更会调用 onUpgrade 函数，在这根据版本号进行升级数据库
        switch (oldVersion) {
            case 1:
                // do something
                break;
            default:
                break;
        }
    }

    public static SQLiteDbHelper getInstance(Context context) {
        if(instance == null){
            instance = new SQLiteDbHelper(context);
        }
        return instance;
    }


    public SQLiteDbHelper(Context context) {
        // 传递数据库名与版本号给父类
        super(context, DB_NAME, null, DB_VERSION);
    }

    @Override
    public void onOpen(SQLiteDatabase db) {
        super.onOpen(db);
        if (!db.isReadOnly()) {
            // 启动外键
            db.execSQL("PRAGMA foreign_keys = 1;");
            //或者这样写
            String query = String.format("PRAGMA foreign_keys = %s", "ON");
            db.execSQL(query);
        }
    }

    public Cursor select(){
        if(db == null){
            db = getReadableDatabase();
        }
        Cursor cursor = db.query(TABLE_ArcReg,null,null,null,null,null,null);
        return cursor;
    }

    private ContentValues faceToContentValues(int id, String name, String path, byte[] feature)
    {
        ContentValues contentValues = new ContentValues();
        contentValues.put("id", id);
        contentValues.put("name", name);
        contentValues.put("path", path);
        contentValues.put("feature", feature);
        return contentValues;
    }
    private   ContentValues faceToContentValues(int id,String name)
    {
        ContentValues contentValues = new ContentValues();
        contentValues.put("id", id);
        contentValues.put("name", name);
        return contentValues;
    }

    public int Delete(String tablename,String fieldname){
        int ret  = -1;
        if(db == null){
            db = getReadableDatabase();
        }
        ret =  db.delete(tablename,"name=?",new String[]{fieldname});
        return ret;
    }

    public long Insert(String userName,String jpgpath,byte[] feature){
        if(db == null){
            db = getWritableDatabase();
        }

        Cursor queryCursor = db.query(TABLE_History,null,null,null,null,null,null);
        int all_number = queryCursor.getCount();
        all_number +=  1;

        long insertHistoryResult = db.insert(TABLE_History,null,
                faceToContentValues(all_number,userName));

        long insertResult = db.insert(SQLiteDbHelper.TABLE_ArcReg,null,
                faceToContentValues(all_number,userName,jpgpath,feature));

        return insertResult;
    }

}
