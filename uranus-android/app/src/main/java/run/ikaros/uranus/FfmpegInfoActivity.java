package run.ikaros.uranus;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

public class FfmpegInfoActivity extends AppCompatActivity {

    // Used to load the 'uranus' library on application startup.
    static {
        System.loadLibrary("uranus");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ffmpeg_info);

        TextView textView = findViewById(R.id.ffmpegTextView);
        textView.setText(getFfmpegVersion());
    }

    private static native String getFfmpegVersion();
}