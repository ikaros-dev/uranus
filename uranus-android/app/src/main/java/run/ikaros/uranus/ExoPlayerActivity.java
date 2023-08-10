package run.ikaros.uranus;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.google.android.exoplayer2.ExoPlayer;
import com.google.android.exoplayer2.MediaItem;
import com.google.android.exoplayer2.ui.PlayerView;

import java.util.Objects;

public class ExoPlayerActivity extends AppCompatActivity {

    private ExoPlayer exoPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_exo_player);

        PlayerView playerView = findViewById(R.id.exoPlayerVideoView);
        exoPlayer = new ExoPlayer.Builder(ExoPlayerActivity.this).build();
        playerView.setPlayer(exoPlayer);
        MediaItem mediaItem = MediaItem.fromUri(VideoTmp.H265_URL);
        exoPlayer.setMediaItem(mediaItem);
        exoPlayer.prepare();
    }

    @Override
    protected void onStop() {
        super.onStop();
        if(Objects.nonNull(exoPlayer)) {
            exoPlayer.release();
        }
    }
}