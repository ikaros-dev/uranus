package run.ikaros.uranus;

public interface VideoPlayer {
    void prepare();
    void start();
    void pause();
    void resume();
    void stop();
    void seek(int seconds);
    void setSpeed(float speed);
}
