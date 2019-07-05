package com.example.zhangzd.cusplayer;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity implements LivePlayer.OnPrepareListener, SeekBar.OnSeekBarChangeListener {
    LivePlayer livePlayer;
    SeekBar seekBar;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        SurfaceView surfaceView = findViewById(R.id.surfaceView);
        seekBar = findViewById(R.id.seekBar);
        checkPermission();
        livePlayer = new LivePlayer(surfaceView.getHolder());
        File file = new File(Environment.getExternalStorageDirectory(), "input2.mp4");
        livePlayer.setDataSource(file.getAbsolutePath());
        livePlayer.setOnPrepareListener(this);
        seekBar.setOnSeekBarChangeListener(this);
    }

    @Override
    public void onPrepare() {
        final int duration = livePlayer.getDuration();
        Log.e("duration","" + duration);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                seekBar.setVisibility(View.VISIBLE);
                seekBar.setMax(duration);
            }
        });

        //初始化FFmpeg成功后，开始解析视频
        livePlayer.start();
    }

    public void play(View view) {
        livePlayer.prepare();

    }


    public void checkPermission() {
        boolean isGranted = true;
        if (android.os.Build.VERSION.SDK_INT >= 23) {
            if (this.checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                //如果没有写sd卡权限
                isGranted = false;
            }
            if (this.checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                isGranted = false;
            }
            Log.i("cbs","isGranted == "+isGranted);
            if (!isGranted) {
                this.requestPermissions(
                        new String[]{Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission
                                .ACCESS_FINE_LOCATION,
                                Manifest.permission.READ_EXTERNAL_STORAGE,
                                Manifest.permission.WRITE_EXTERNAL_STORAGE},
                        102);
            }
        }

    }


    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

    }


    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }


    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        int seekTime = livePlayer.getDuration() * seekBar.getProgress() * 100;
        livePlayer.set

    }
}
