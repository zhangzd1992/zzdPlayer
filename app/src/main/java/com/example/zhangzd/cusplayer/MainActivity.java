package com.example.zhangzd.cusplayer;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    LivePlayer livePlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        SurfaceView surfaceView = findViewById(R.id.surfaceView);
        livePlayer = new LivePlayer(surfaceView.getHolder());
        livePlayer.setDataSource("");
    }


    public void play(View view) {
        livePlayer.start();

    }
}
