package com.example.zhangzd.cusplayer;

import android.view.Surface;
import android.view.SurfaceHolder;

/**
 * @Description:
 * @Author: zhangzd
 * @CreateDate: 2019-06-26 10:45
 */
public class LivePlayer implements SurfaceHolder.Callback{
    static {
        System.loadLibrary("native-lib");
    }

    private SurfaceHolder surfaceHolder;
    private String dataSource;

    public LivePlayer(SurfaceHolder holder) {
        if(null != this.surfaceHolder) {
            this.surfaceHolder.removeCallback(this);
        }
        this.surfaceHolder = holder;
        holder.addCallback(this);
    }


    /**
     * 设置文件或者流路径
     * @param dataSource
     */
    public void setDataSource(String dataSource) {
        this.dataSource = dataSource;
    }


    //准备开启播放
    public void prepare() {
        native_prepare(dataSource);
    }

    public void start() {

    }





    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        native_set_surface(holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {}

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {}


    /************************************* native方法声明 ***********************************************/

    public native void native_start();
    public native void native_set_surface(Surface surface);
    public native void native_prepare(String dataSource);
}
