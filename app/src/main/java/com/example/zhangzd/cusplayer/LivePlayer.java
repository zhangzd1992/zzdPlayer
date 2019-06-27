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
    private OnPrepareListener prepareListener;
    private OnErrorListener errorListener;
    private OnProgressListener progressListener;

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
        native_start();
    }

    public interface OnPrepareListener {
         void onPrepare();
    }
    public interface OnErrorListener {
        void onError(int errorCode);
    }

    public interface OnProgressListener {
        void onProgress(int progress);
    }


    public void setOnPrepareListener(OnPrepareListener listener) {
        this.prepareListener = listener;
    }
    public void setOnErrorListener(OnErrorListener listener) {
        this.errorListener = listener;
    }

    public void setOnProgressListener(OnProgressListener listener) {
        this.progressListener = listener;
    }

    public void onPrepare(){
        if (null != prepareListener) {
            prepareListener.onPrepare();
        }
    }

    public void onError(int errorCode) {
        if (null != errorListener) {
            errorListener.onError(errorCode);
        }
    }


    public void onProgress(int progerss){
        if (null != progressListener) {
            progressListener.onProgress(progerss);
        }
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
