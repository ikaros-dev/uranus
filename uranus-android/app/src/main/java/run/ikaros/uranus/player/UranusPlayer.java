package run.ikaros.uranus.player;

import android.util.Log;

import com.google.android.exoplayer2.Player;

import java.util.List;

import run.ikaros.uranus.listener.PlayerListener;
import run.ikaros.uranus.listener.PlayerPreparedListener;
import run.ikaros.uranus.opengl.UranusGLSurfaceView;

public class UranusPlayer implements VideoPlayer {
    // Used to load the 'uranus' library on application startup.
    static {
        System.loadLibrary("uranus");
    }

    private static final String TAG = UranusPlayer.class.getSimpleName();

    private UranusGLSurfaceView surfaceView;
    private PlayerListener playerListener;
    private PlayerPreparedListener playerPreparedListener;
    private int duration = 0;
    private int currentSourceIndex = 0;
    private List<String> sources;

    public int getDuration() {
        return duration;
    }

    public void setSurfaceView(UranusGLSurfaceView surfaceView) {
        this.surfaceView = surfaceView;
    }

    public void setPlayerListener(PlayerListener playerListener) {
        this.playerListener = playerListener;
    }

    public void setPlayerPreparedListener(PlayerPreparedListener playerPreparedListener) {
        this.playerPreparedListener = playerPreparedListener;
    }

    public void setSources(List<String> sources) {
        this.sources = sources;
    }

    public void setSources(String source) {
        setSources(List.of(source));
    }

    private native void delegatePrepare(String source);

    private native void delegateStart();

    private native void delegatePause();

    private native void delegateResume();

    private native void delegateStop();

    private native void delegateSeek(int seconds);

    private native void delegateVolume(int percent);

    private native void delegateSpeed(float speed);

    private native void delegatePitch(float pitch);

    private native void delegateMute(int mute);

    @Override
    public void prepare() {
        if (sources == null || sources.isEmpty()) {
            Log.w(TAG, "source must no be empty");
            return;
        }

        new Thread(() -> delegatePrepare(sources.get(currentSourceIndex))).start();
    }

    @Override
    public void start() {
        if (sources == null || sources.isEmpty()) {
            Log.w(TAG, "source must no be empty");
            return;
        }


        new Thread(this::delegateStart).start();
    }

    public void onCallPrepared() {
        if (playerPreparedListener != null) {
            playerPreparedListener.onPrepared();
        }
    }

    public void onCallRenderYUV(int width, int height, byte[] y, byte[] u, byte[] v) {
        if (this.surfaceView != null) {
            this.surfaceView.setYUVData(width, height, y, u, v);
        }
    }

    public void onCallTimeInfo(int currentTime, int totalTime) {
        if (playerListener == null) {
            return;
        }
        duration = totalTime;
        playerListener.onCurrentTime(currentTime, totalTime);
    }

    public void onCallLoad(boolean load)
    {

    }

    @Override
    public void pause() {
        delegatePause();
    }

    @Override
    public void resume() {
        delegateResume();
    }


    @Override
    public void stop() {
        new Thread(this::delegateStop).start();
    }

    @Override
    public void seek(int seconds) {
        delegateSeek(seconds);
    }

    @Override
    public void volume(int percent) {
        delegateVolume(percent);
    }

    @Override
    public void setSpeed(float speed) {
        delegateSpeed(speed);
    }

    @Override
    public void pitch(float pitch) {
        delegatePitch(pitch);
    }

    @Override
    public void mute(int mute) {
        delegateMute(mute);
    }
}
