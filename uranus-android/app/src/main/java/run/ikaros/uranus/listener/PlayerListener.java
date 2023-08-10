package run.ikaros.uranus.listener;

public interface PlayerListener {
    void onLoad(boolean load);
    void onCurrentTime(int currentTime,int totalTime);
    void onError(int code, String msg);
    void onPause(boolean pause);
    void onComplete();
    String onNext();
}
