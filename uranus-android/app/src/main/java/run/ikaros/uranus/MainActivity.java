package run.ikaros.uranus;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import run.ikaros.uranus.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'uranus' library on application startup.
    static {
        System.loadLibrary("uranus");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());

        Button button = findViewById(R.id.button);
        button.setText("exoPlayer");
        button.setOnClickListener(v ->
                startActivity(new Intent(MainActivity.this, ExoPlayerActivity.class)));

        Button ffmpegBtn = findViewById(R.id.ffmpegBtn);
        ffmpegBtn.setText("to get ffmpeg info");
        ffmpegBtn.setOnClickListener(v ->
                startActivity(new Intent(MainActivity.this, FfmpegInfoActivity.class)));

    }

    /**
     * A native method that is implemented by the 'uranus' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}