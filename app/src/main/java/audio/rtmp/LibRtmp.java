package audio.rtmp;

/**
 * Created by zhonglz on 2016/12/8.
 */
public class LibRtmp {
    private final static String TAG = LibRtmp.class.getSimpleName();

    static {
        System.loadLibrary("vrtmp");
    }

    public native int               initRtmp(String url);
    public native int               sendAudio(byte[] inputbuffer, int bufferSize, int isKeyFrame, int ts);
    public native int               sendAudioSpec(byte[] inputbuffer, int bufferSize);
    public native int               uninitRtmp();
    public native void              setDebug(boolean isDebug);
}
