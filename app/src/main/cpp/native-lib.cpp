#include <string>

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf("(^_^) " format "\n", ##__VA_ARGS__)
#endif


//Output FFmpeg's av_log()
void custom_log(void *ptr, int level, const char* fmt, va_list vl){
    FILE *fp=fopen("/storage/emulated/0/av_log.txt","a+");
    if(fp){
        vfprintf(fp,fmt,vl);
        fflush(fp);
        fclose(fp);
    }
}



extern  "C"{
#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "include/libswscale/swscale.h"
#include "include/libavutil/log.h"
#include "include/libavutil/imgutils.h"
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_leesunr_ndktest_MainActivity_stringFromJNI(JNIEnv* env, jobject obj, jstring msg) {
    std::string hello = env->GetStringUTFChars(msg, 0);
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_leesunr_ndktest_MainActivity_getCodec(JNIEnv *env, jobject obj, jstring input_jstr, jstring output_jstr) {

    AVFormatContext	*pFormatCtx;
    AVCodecContext	*pCodecCtx;
    AVCodec			*pCodec;
    uint8_t *out_buffer;
    AVPacket *packet;
    AVFrame	*pFrame,*pFrameYUV;

    struct SwsContext *img_convert_ctx;
    av_log_set_callback(custom_log); //로그
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    int i, videoindex, code=0;
    char input_str[500]={0};
    char output_str[500]={0};
    char info[1000]={0};
    sprintf(input_str,"%s",env->GetStringUTFChars(input_jstr, NULL));
    sprintf(output_str,"%s",env->GetStringUTFChars(output_jstr, NULL));
    if((code = avformat_open_input(&pFormatCtx,input_str,NULL,NULL))!=0){
        LOGE("Couldn't open input stream. %s %d\n",input_str, code);
    }
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        LOGE("Couldn't find stream information.\n");
    }

    videoindex=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
            videoindex=i;
            break;
        }
    if(videoindex==-1){
        LOGE("Couldn't find a video stream.\n");
    }

    pCodecCtx=pFormatCtx->streams[videoindex]->codec;
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL){
        LOGE("Couldn't find Codec.\n");
    }
    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
        LOGE("Couldn't open codec.\n");
    }

    pFrame=av_frame_alloc();
    pFrameYUV=av_frame_alloc();
    out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  pCodecCtx->width, pCodecCtx->height,1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
                         AV_PIX_FMT_YUV420P,pCodecCtx->width, pCodecCtx->height,1);

    packet=(AVPacket *)av_malloc(sizeof(AVPacket));

    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    sprintf(info,   "[Input     ]%s\n", input_str);
    sprintf(info, "%s[Output    ]%s\n",info,output_str);
    sprintf(info, "%s[Format    ]%s\n",info, pFormatCtx->iformat->name);
    sprintf(info, "%s[Codec     ]%s\n",info, pCodecCtx->codec->name);
    sprintf(info, "%s[Resolution]%dx%d\n",info, pCodecCtx->width,pCodecCtx->height);
    std::string hello = info;
    return env->NewStringUTF(hello.c_str());

}
