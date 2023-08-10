package run.ikaros.uranus;

import android.opengl.GLSurfaceView;
import android.util.Log;

import java.util.List;

import run.ikaros.uranus.listener.PlayerListener;
import run.ikaros.uranus.listener.PlayerPreparedListener;

public class UranusPlayer implements VideoPlayer{
    // Used to load the 'uranus' library on application startup.
    static {
        System.loadLibrary("uranus");
    }

    private static final String TAG = UranusPlayer.class.getSimpleName();

    private GLSurfaceView surfaceView;
    private PlayerListener playerListener;
    private PlayerPreparedListener playerPreparedListener;
    private int duration = 0;
    private int currentSourceIndex = 0;
    private List<String> sources;

    public int getDuration() {
        return duration;
    }

    public void setSurfaceView(GLSurfaceView surfaceView) {
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


    @Override
    public void prepare() {
        if(sources == null || sources.isEmpty()) {
            Log.w(TAG, "source must no be empty");
            return;
        }

        new Thread(() -> delegatePrepare(sources.get(currentSourceIndex))).start();
    }

    private native void delegatePrepare(String source);

    @Override
    public void start() {
        if(sources == null || sources.isEmpty()) {
            Log.w(TAG, "source must no be empty");
            return;
        }


        new Thread(this::delegateStart).start();
    }

    @Override
    public void pause() {

    }

    @Override
    public void resume() {

    }

    private native void delegateStart();

    @Override
    public void stop() {

    }

    @Override
    public void seek(int seconds) {

    }

    @Override
    public void setSpeed(float speed) {

    }
}
