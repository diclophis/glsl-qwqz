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


class DemoGLSurfaceView extends GLSurfaceView {


  private DemoRenderer mRenderer;


  public DemoGLSurfaceView(Context context) {
    super(context);
    setEGLContextClientVersion(2);
    //setDebugFlags(DEBUG_CHECK_GL_ERROR | DEBUG_LOG_GL_CALLS);
    mRenderer = new DemoRenderer(context);
    setRenderer(mRenderer);
  }


  @Override
  public boolean onTouchEvent(final MotionEvent event) {
    int index = event.getActionMasked();
    int pointerId = event.getActionIndex();
    float x = 0;
    float y = 0;
    x = event.getX(pointerId);
    y = event.getY(pointerId);

    if (index == MotionEvent.ACTION_POINTER_UP || index == MotionEvent.ACTION_UP || index == MotionEvent.ACTION_CANCEL) {
      nativeTouch(x, y, 2);
    } else if (index == MotionEvent.ACTION_MOVE) {
      nativeTouch(x, y, 1);
    } else if (index == MotionEvent.ACTION_POINTER_DOWN || index == MotionEvent.ACTION_DOWN) {
      nativeTouch(x, y, 0);
    }

    return true;
  }


  @Override
  public void onPause() {
    Log.v(this.toString(), "DemoGLSurfaceView::onPause");
    super.onPause();
    nativePause();
  }


  @Override
  public void onResume() {
    Log.v(this.toString(), "DemoGLSurfaceView::onResume");
    super.onResume();
    nativeResume();
  }


  private static native void nativePause();
  private static native void nativeResume();
  private static native void nativeTouch(float x, float y, int hitState);
}
