// Jon Bardin GPL

package com.risingcode.qwqz;


import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.app.Activity;
import android.content.DialogInterface;
import android.app.AlertDialog;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.View;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebChromeClient;
import android.webkit.WebViewClient;
import android.util.Log;
import java.util.Queue;
import java.util.LinkedList;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.content.res.AssetManager;
import android.opengl.GLUtils;
import android.opengl.GLES10;
import android.content.res.Configuration;
import java.io.InputStream;
import java.io.IOException;
import android.view.ViewGroup.LayoutParams;
import android.graphics.Color;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.net.URI;
import java.net.URL;
import java.net.URLEncoder;
import java.io.UnsupportedEncodingException;

import com.risingcode.qwqz.DemoRenderer;
import com.risingcode.qwqz.DemoGLSurfaceView;


public class DemoActivity extends Activity {

  protected static AudioTrack at1;
	private DemoGLSurfaceView mGLView;

	private native int initNative(AssetManager am);
  private static native void setMinBuffer(int size);

  static {
    System.loadLibrary("qwqz");
  }

  public static void writeAudio(short[] bytes, int offset, int size) {
    //if (at1.getPlayState() != AudioTrack.PLAYSTATE_PLAYING) {
    //  at1.play();
    //}
    //int written = at1.write(bytes, offset, size);
  }


  @Override
  protected void onDestroy() {
    Log.v("ClearActivity", ":::::: instance" + this + " onDestroy: is called");
    super.onDestroy();
  }


	@Override
	protected void onCreate(Bundle savedInstanceState) {
    Log.v(this.toString(), "DemoActivity::onCreate!!!!!!");
		super.onCreate(savedInstanceState);
    //setRequestedOrientation(getResources().getConfiguration().orientation);
    getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);   
		mGLView = new DemoGLSurfaceView(this);
		setContentView(mGLView);

    /*
    int rate = 44100;
    int min = AudioTrack.getMinBufferSize(rate, AudioFormat.CHANNEL_CONFIGURATION_STEREO, AudioFormat.ENCODING_PCM_16BIT);
    setMinBuffer(min / 16);
    at1 = new AudioTrack(AudioManager.STREAM_MUSIC, rate, AudioFormat.CHANNEL_CONFIGURATION_STEREO, AudioFormat.ENCODING_PCM_16BIT, min * 16, AudioTrack.MODE_STREAM);
    at1.setStereoVolume(1.0f, 1.0f);
    */

/*
    try {
      //android.content.res.AssetFileDescriptor afd = getAssets().openFd("sound/music.mp3");
      //android.media.MediaPlayer player = new android.media.MediaPlayer();
      //player.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
      //player.prepare();
      //player.setVolume(1f, 1f);
      //player.setLooping(true);
      //player.start();
    } catch (java.io.IOException e) {
      Log.v(this.toString(), e.toString() + "WTF!!!!!!!!!!!!!!");
    }
*/

    int res = initNative(getAssets());
	}


  public boolean pushMessageToWebView(String messageToPush) {
    return true;
  }


  public String popMessageFromWebView() {
    return "";
  }


  public void onConfigurationChanged(Configuration newConfig) {
    Log.v(this.toString(), "DemoActivity::onConfigurationChange!!!!!!");
    super.onConfigurationChanged(newConfig);
  }


  @Override
  protected void onPause() {
    Log.v(this.toString(), "DemoActivity::onPause!!!!!!" + mGLView);
    super.onPause();
    mGLView.onPause();
  }


  @Override
  protected void onResume() {
    Log.v(this.toString(), "DemoActivity::onResume!!!!!!" + mGLView);
    super.onResume();
    mGLView.onResume();
  }
}
