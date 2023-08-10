package run.ikaros.uranus;

import android.content.res.Resources;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.provider.CalendarContract;

import androidx.appcompat.app.AppCompatActivity;

import com.google.android.exoplayer2.C;
import com.google.android.exoplayer2.ExoPlayer;
import com.google.android.exoplayer2.MediaItem;
import com.google.android.exoplayer2.ui.AspectRatioFrameLayout;
import com.google.android.exoplayer2.ui.StyledPlayerView;
import com.google.android.exoplayer2.ui.SubtitleView;
import com.google.android.exoplayer2.util.MimeTypes;
import com.google.common.collect.ImmutableList;

import java.util.Objects;

public class ExoPlayerActivity extends AppCompatActivity {

    private ExoPlayer exoPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_exo_player);

        ExoPlayerView playerView = findViewById(R.id.exoPlayerVideoView);
        exoPlayer = new ExoPlayer.Builder(ExoPlayerActivity.this).build();
        playerView.setPlayer(exoPlayer);

        MediaItem mediaItem = MediaItem.fromUri(VideoTmp.EMBED_ASS_H265_URL);

        // MediaItem.SubtitleConfiguration subtitle =
        //         new MediaItem.SubtitleConfiguration.Builder(Uri.parse(VideoTmp.H265_CHS_ASS_URL))
        //                 .setMimeType(MimeTypes.APPLICATION_SUBRIP) // The correct MIME type (required).
        //                 .setLanguage("CHS") // The subtitle language (optional).
        //                 // .setSelectionFlags() // Selection flags for the track (optional).
        //                 .build();
        // MediaItem mediaItem  = new MediaItem.Builder()
        //         .setUri(VideoTmp.H265_URL)
        //         .setSubtitleConfigurations(ImmutableList.of(subtitle))
        //         .build();

        exoPlayer.setMediaItem(mediaItem);
        exoPlayer.prepare();
        playerView.setControllerHideOnTouch(false);
        playerView.setShowBuffering(StyledPlayerView.SHOW_BUFFERING_WHEN_PLAYING);
        playerView.setResizeMode(AspectRatioFrameLayout.RESIZE_MODE_FIT);
        playerView.setBackgroundColor(Color.BLACK);
        playerView.setKeepContentOnPlayerReset(true);
        playerView.setShowRewindButton(true);
        playerView.setShowFastForwardButton(true);
        playerView.setShowPreviousButton(true);
        playerView.setShowSubtitleButton(true);
        playerView.setShowMultiWindowTimeBar(true);
        playerView.setShowVrButton(false);

        SubtitleView subtitleView = playerView.getSubtitleView();
    }

    @Override
    protected void onStop() {
        super.onStop();
        if (Objects.nonNull(exoPlayer)) {

            exoPlayer.release();
        }
    }
}