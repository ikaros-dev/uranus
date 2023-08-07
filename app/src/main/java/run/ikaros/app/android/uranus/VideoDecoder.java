package run.ikaros.app.android.uranus;

import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;

import java.nio.ByteBuffer;

public class VideoDecoder implements Runnable{
    private MediaCodec mediaCodec;
    private final String url;

    public VideoDecoder(MediaCodec mediaCodec, String url) {
        this.mediaCodec = mediaCodec;
        this.url = url;
    }


    @Override
    public void run() {
        MediaExtractor extractor = new MediaExtractor();
        try {
            extractor.setDataSource(url);

//            int videoTrackIndex = getVideoTrackIndex(extractor);
//            MediaFormat videoFormat = extractor.getTrackFormat(videoTrackIndex);
//            String mimeType = videoFormat.getString(MediaFormat.KEY_MIME);
//
//            mediaCodec = MediaCodec.createDecoderByType(mimeType);
//            mediaCodec.configure(videoFormat, surface, null, 0);
//            mediaCodec.start();

//            ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();
//            ByteBuffer[] outputBuffers = mediaCodec.getOutputBuffers();
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();

            boolean isInputDone = false;
            boolean isOutputDone = false;

            while (!isOutputDone) {
                if (!isInputDone) {
                    int inputBufferIndex = mediaCodec.dequeueInputBuffer(10000);
                    if (inputBufferIndex >= 0) {
                        ByteBuffer inputBuffer =  mediaCodec.getInputBuffer(inputBufferIndex);
                        int sampleSize = extractor.readSampleData(inputBuffer, 0);

                        if (sampleSize < 0) {
                            mediaCodec.queueInputBuffer(inputBufferIndex, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                            isInputDone = true;
                        } else {
                            long presentationTimeUs = extractor.getSampleTime();
                            mediaCodec.queueInputBuffer(inputBufferIndex, 0, sampleSize, presentationTimeUs, 0);
                            extractor.advance();
                        }
                    }
                }

                int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 1000);
                if (outputBufferIndex >= 0) {
                    if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                        isOutputDone = true;
                    }

//                    ByteBuffer outputBuffer = mediaCodec.getOutputBuffer(outputBufferIndex);
//                    // 处理解码后的视频数据，例如
//                    renderOutputBuffer(outputBuffer);


                    mediaCodec.releaseOutputBuffer(outputBufferIndex, true);
                } else if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    MediaFormat newFormat = mediaCodec.getOutputFormat();
                    // 可以在这里获取新的格式信息，例如视频宽高等
                }
            }

        } catch (Exception e) {
            Log.e("VideoDecoder", "decode video data fail: ",  e);
        } finally {
            mediaCodec.stop();
            mediaCodec.release();

            extractor.release();
        }

    }

//    private int getVideoTrackIndex(MediaExtractor extractor) {
//        for (int i = 0; i < extractor.getTrackCount(); i++) {
//            MediaFormat format = extractor.getTrackFormat(i);
//            String mimeType = format.getString(MediaFormat.KEY_MIME);
//            if (mimeType.startsWith("video/")) {
//                return i;
//            }
//        }
//        return -1;
//    }

    private void renderOutputBuffer(ByteBuffer outputBuffer) {
        // 将解码后的视频数据渲染到Surface上
        // 这里可以使用OpenGL、SurfaceView或其它渲染方式进行处理
    }
}
