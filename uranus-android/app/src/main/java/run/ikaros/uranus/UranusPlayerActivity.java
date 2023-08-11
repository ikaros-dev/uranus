package run.ikaros.uranus;

import androidx.appcompat.app.AppCompatActivity;

import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

import run.ikaros.uranus.listener.PlayerListener;
import run.ikaros.uranus.listener.PlayerPreparedListener;

public class UranusPlayerActivity extends AppCompatActivity {

    private TextView tvTime;
    private GLSurfaceView glSurfaceView;
    private SeekBar seekBar;
    private int position;
    private boolean seek = false;
    private UranusPlayer player;
    private String videoUrl = VideoTmp.H264_URL;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_uranus_player);

        tvTime = findViewById(R.id.tv_time);
        glSurfaceView = findViewById(R.id.gsSurfaceView);
        seekBar = findViewById(R.id.seekbar);

        player = new UranusPlayer();
        player.setSurfaceView(glSurfaceView);
        player.setPlayerListener(new PlayerListener() {
            @Override
            public void onLoad(boolean load) {

            }

            @Override
            public void onCurrentTime(int currentTime, int totalTime) {
                if(!seek &&totalTime> 0)
                {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            seekBar.setProgress(currentTime* 100 / totalTime);
                            tvTime.setText( secondsToDateFormat(currentTime)
                                    + "/" + secondsToDateFormat( totalTime));
                        }
                    });

                }
            }

            @Override
            public void onError(int code, String msg) {

            }

            @Override
            public void onPause(boolean pause) {

            }

            @Override
            public void onComplete() {

            }

            @Override
            public String onNext() {
                return null;
            }
        });

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                position = progress * player.getDuration() / 100;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                seek = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                player.seek(position);
                seek = false;
            }
        });

    }

    public static String secondsToDateFormat(int sends ) {
        long hours = sends / (60 * 60);
        long minutes = (sends % (60 * 60)) / (60);
        long seconds = sends % (60);

        String sh = "00";
        if (hours > 0) {
            if (hours < 10) {
                sh = "0" + hours;
            } else {
                sh = hours + "";
            }
        }
        String sm = "00";
        if (minutes > 0) {
            if (minutes < 10) {
                sm = "0" + minutes;
            } else {
                sm = minutes + "";
            }
        }

        String ss = "00";
        if (seconds > 0) {
            if (seconds < 10) {
                ss = "0" + seconds;
            } else {
                ss = seconds + "";
            }
        }
        return sm + ":" + ss;

    }

    public void begin(View view) {
        player.setPlayerPreparedListener(new PlayerPreparedListener() {
            @Override
            public void onPrepared() {
                Log.i(this.getClass().getSimpleName(), "start play video");
                player.start();
            }
        });
        player.setSources(videoUrl);
        player.prepare();
    }

    public void stop(View view) {
        player.stop();
    }

    public void pause(View view) {
        player.pause();
    }

    public void resume(View view) {
        player.resume();
    }

    public void speed1(View view) {
        player.setSpeed(1F);
    }

    public void speed2(View view) {
        player.setSpeed(2F);
    }
}