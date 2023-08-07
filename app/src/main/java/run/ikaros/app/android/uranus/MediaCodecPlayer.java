package run.ikaros.app.android.uranus;

import android.content.Context;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;

public class MediaCodecPlayer {
    private static final String TAG = "MediaCodecPlayer";

    private final String url;
    private final Surface surface;
    private final Context context;

    private MediaCodec mediaCodec;


    public MediaCodecPlayer(String url, Surface surface, Context context) {
        this.url = url;
        this.surface = surface;
        this.context = context;

        MediaExtractor mediaExtractor = new MediaExtractor();
        try {
            mediaExtractor.setDataSource(url);
        } catch (IOException e) {
            Log.e(TAG, "current source incorrect, url: " + url);
            throw new RuntimeException(e);
        }

        String type = null;
        MediaFormat mediaFormat = null;
        for (int i = 0; i < mediaExtractor.getTrackCount(); i++) {
            MediaFormat trackFormat = mediaExtractor.getTrackFormat(i);
            String mimeType = trackFormat.getString(MediaFormat.KEY_MIME);
            if(mimeType != null && mimeType.startsWith("video/")) {
                type = mimeType;
                mediaFormat = trackFormat;
                break;
            }
        }

        if(type == null) {
            Log.e(TAG, "current source is not video, url: " + url);
            return;
        }

        try {
            mediaCodec = MediaCodec.createDecoderByType(type);
        } catch (Exception e) {
            Log.e(TAG, "current source is not support mediacodec, url: " + url);
            return;
        }

        mediaCodec.configure(mediaFormat, surface, null, 0);

    }

    public void play() {
        mediaCodec.start();
        new Thread(new VideoDecoder(mediaCodec, url)).start();
    }

    public void release() {
        mediaCodec.flush();
        mediaCodec.release();
    }
}
