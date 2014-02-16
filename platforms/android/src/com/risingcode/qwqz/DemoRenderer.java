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


class DemoRenderer implements GLSurfaceView.Renderer {
  Context mContext;

  public DemoRenderer(Context context) {
    mContext = context;
  }


  public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    nativeOnSurfaceCreated();
  }


  public void onSurfaceChanged(GL10 gl, int w, int h) {
    nativeResize(w, h);
  }


  public void onDrawFrame(GL10 gl) {
    nativeRender();
  }


  private native void nativeOnSurfaceCreated();
  private static native void nativeResize(int w, int h);
  private static native void nativeRender();
}
