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
    /*
    try {
      AssetManager am = mContext.getAssets();
      String path = "textures";
      String[] texture_file_names = am.list(path);
      int[] textures = new int[texture_file_names.length];
      int[] tmp_tex = new int[texture_file_names.length];
      gl.glGenTextures(texture_file_names.length, tmp_tex, 0);
      int glError;
      if ((glError = gl.glGetError()) != 0) {
        Log.v(this.toString(), "unable to glGenTextures");
      }

      textures = tmp_tex; 
      for (int i=0; i<texture_file_names.length; i++) {
        InputStream stream = am.open(path + "/" + texture_file_names[i]);
        Bitmap bitmap = BitmapFactory.decodeStream(stream);
        int t = textures[i];
        gl.glBindTexture(GL10.GL_TEXTURE_2D, t);
        //gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
        //gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_NEAREST);
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
        GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);
        if ((glError = gl.glGetError()) != 0) {
          Log.v(this.toString(), "unable to GLUtils.texImage2D");
        }
        gl.glBindTexture(GL10.GL_TEXTURE_2D, 0);
      }
      //Log.v(this.toString(), "DOES THIS HAPPEN TWICE??");
      nativeOnSurfaceCreated(texture_file_names.length, textures);
    } catch(IOException e) {
      Log.v(this.toString(), e.toString());
    }
    */
    nativeOnSurfaceCreated(); //texture_file_names.length, textures);
  }


  public void onSurfaceChanged(GL10 gl, int w, int h) {
    nativeResize(w, h);
  }


  public void onDrawFrame(GL10 gl) {
    nativeRender();
  }


  private native void nativeOnSurfaceCreated(); //int count, int[] textures);
  private static native void nativeResize(int w, int h);
  private static native void nativeRender();


}
