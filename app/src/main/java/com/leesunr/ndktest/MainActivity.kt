package com.leesunr.ndktest

import android.content.Intent
import android.os.Bundle
import android.os.Environment
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import java.lang.Exception

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val folderurl: String = Environment.getExternalStorageDirectory().absolutePath
        val urltext_input: String = "test.mp4"
        val inputurl = "$folderurl/Download/$urltext_input"

        // Example of a call to a native method
        //sample_text.text = getCodec(inputurl,"")
        //sample_text.text = getCodec("http://192.168.35.235/test.mp4","")


        btn_add.setOnClickListener {
            var intent = Intent(this,FileSelectActivity::class.java)
            startActivity(intent)
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(input:String): String
    external fun getCodec(input:String,output:String): String

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")

        }
    }
}
