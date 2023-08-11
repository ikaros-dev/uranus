package run.ikaros.uranus.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class UranusGLSurfaceView extends GLSurfaceView {
    private UranusGLSurfaceViewRenderer renderer;

    public UranusGLSurfaceView(Context context) {
        this(context, null);
    }

    public UranusGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setEGLContextClientVersion(2);
        renderer = new UranusGLSurfaceViewRenderer(context);
        setRenderer(renderer);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    public void setYUVData(int width, int height, byte[] y, byte[] u, byte[] v) {
        if (renderer != null) {
            renderer.setYUVRenderData(width, height, y, u, v);
            requestRender();
        }

    }

}
