package run.ikaros.uranus.player;

public interface VideoPlayer {
    void prepare();
    void start();
    void pause();
    void resume();
    void stop();
    void seek(int seconds);
    void volume(int percent);
    void setSpeed(float speed);
    void pitch(float pitch);
    void mute(int mute);
}
