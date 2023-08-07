package run.ikaros.app.android.uranus;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'uranus' library on application startup.
    static {
        System.loadLibrary("uranus");
    }

    private MediaCodecPlayer mediaCodecPlayer;

    public void checkPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && checkSelfPermission(
                android.Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(new String[]{
                    android.Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE
            }, 1);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
//        checkPermission();
        initSurface();
    }

    private void initSurface() {
        SurfaceView surface = findViewById(R.id.videoView);
        final SurfaceHolder surfaceHolder = surface.getHolder();
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                mediaCodecPlayer = new MediaCodecPlayer(VideoTmp.H265_URL, surfaceHolder.getSurface(),
                        MainActivity.this);
            }
            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
            }
            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                mediaCodecPlayer.release();
            }
        });

    }

    public void play(View view) {
        mediaCodecPlayer.play();
    }

    /**
     * A native method that is implemented by the 'uranus' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}