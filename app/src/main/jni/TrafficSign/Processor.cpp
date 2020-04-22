#include "Processor.h"

#include "Debug.h"
#include "Utils.h"

#include <vector>


Processor::Processor() {  }

Processor::~Processor()
{
    pthread_exit(NULL);
}


// Prepare class for process
bool Processor::Initialize(JNIEnv *env, jobject obj)
{
    isProcessing = true;

    // Inicia os atributos das threads
    pthread_mutex_init(&m_img_mutex, NULL);
    pthread_cond_init(&m_img_signal, NULL);
    pthread_attr_init(&m_img_attr);

    // Configura as threads como joinable
    pthread_attr_setdetachstate(&m_img_attr, PTHREAD_CREATE_JOINABLE);

    //num_threads = android_getCpuCount();

    num_threads = 3;

    m_threads = new pthread_t[num_threads];

    // Inicia as threads
    for(int i = 0; i < num_threads; ++i)
    {
        pthread_create(&m_threads[i], &m_img_attr, doProcess, (void *)i);
    }

    env->GetJavaVM(&m_javaVM);

    jclass eventClass = reinterpret_cast<jclass>(env->FindClass("com/example/adas/Service"));
    m_eventClass = reinterpret_cast<jclass>(env->NewGlobalRef(eventClass));
    env->DeleteLocalRef(eventClass);

    m_jObject = reinterpret_cast<jobject>(env->NewGlobalRef(obj));
    env->DeleteLocalRef(obj);

    m_onCallBackID = env->GetStaticMethodID(m_eventClass, "onCallBack", "(I)V");

    return true;
}

// Destroy class attributes
bool Processor::Finalize()
{
    // Encerra o processamento
    isProcessing = false;

    // Limpa o buffer
    m_images.clear();

    // Aguarda o fim das threads
    for(int i = 0; i < num_threads; ++i)
    {
        pthread_join(m_threads[i], NULL);
    }

    delete [] m_threads;

    // Destroy os atributos das threads
    pthread_mutex_destroy(&m_img_mutex);
    pthread_cond_destroy(&m_img_signal);
    pthread_attr_destroy(&m_img_attr);

    return true;
}


bool Processor::EventN2J(int value){

    bool threadAttached = false;

    JNIEnv *env;

    int status =  m_javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);

    if (status < 0)
    {
        //Invalid environment got by JavaVM. Trying the current thread...
        status = m_javaVM->AttachCurrentThread(&env,NULL);
        if (status < 0)
        {
            return false;
        }
        threadAttached = true;
    }

    if (!env)
    {
        if (threadAttached)
        {
            m_javaVM->DetachCurrentThread();
        }
        return false;
    }

    env->CallStaticVoidMethod(m_eventClass,m_onCallBackID, value);

    if (threadAttached)
    {
        m_javaVM->DetachCurrentThread();
    }
    return true;
}


void Processor::PushImage(cv::Mat &img)
{
    // bloqueia a area de acesso mutuo
    pthread_mutex_lock(&m_img_mutex);

    if(m_images.size() <= 5)
    {
        m_images.push_back(img);
    }
    else
    {
        if(control_frame <= 0)
        {
            control_frame = 5;
            m_images.push_back(img);

            if(m_images.size() >= BUFFER_SIZE)
            {
                m_images.pop_front();
            }
        }
        else
        {
            control_frame--;
        }

    }

    // Envia um sinal para as threads saberem que há imagem para processar
    if(!m_images.empty())
    {
        pthread_cond_signal(&m_img_signal);
    }

    // Desbloqueia a area de acesso mutuo
    pthread_mutex_unlock(&m_img_mutex);
}

bool Processor::PopImage(cv::Mat &img)
{

    // bloqueia a area de acesso mutuo
    pthread_mutex_lock(&m_img_mutex);

    if(m_images.empty())
    {
        pthread_mutex_unlock(&m_img_mutex);
        return false;
    }

    img = m_images.front();
    m_images.pop_front();

    // Desbloqueia a area de acesso mutuo
    pthread_mutex_unlock(&m_img_mutex);

    return true;
}


void Processor::handleNotification(cv::Mat &mSrc)
{

    PushImage(mSrc);

    // Liberar memória
    mSrc.release();
}


/* Processar a imagem e notificar a camada nativa quando o processamento terminar.
 * chamar este método dentro de uma thread porque ele é cpu bound. */
void * Processor::doProcess(void * id)
{
    long my_id = (long)id;

    while(isProcessing){
        cv::Mat mRes;
        cv::Mat mSub;

        bool ret = m_processorInstance->PopImage(mSub);

        if(!ret)
        {
            pthread_cond_wait(&m_img_signal, &m_img_mutex);
            pthread_mutex_unlock(&m_img_mutex);
            continue;
        }

        // Métodos de segmentação da cor vemelha
        //m_processorInstance->RGB_Zaklouta_2014(mSub, mRes);

        // Método para detectar círculos
        //std::vector<cv::Vec3f> circles = m_processorInstance->DetectCircles(mRes);

        //for( size_t i = 0; i < circles.size(); ++i )
        //{
            /* Notifica a parte em java
            if(num > 0 && num % 10 == 0 && num <= 120){
                m_processorInstance->EventN2J(num);
            }
            */

            // Libera memória.
            //plate_shadow.release();
            //plate.release();
            //delete [] vel;
        //}

        mRes.release();
        mSub.release();
    }
    pthread_exit(NULL);
}


// Control frames persisted
int Processor::control_frame = 0;

bool Processor::isProcessing = false;

// Static members initialization
Processor * Processor::m_processorInstance = NULL;

// Armazena as imagens
std::deque<cv::Mat> Processor::m_images;

int Processor::num_threads = 1;

// pthread
pthread_mutex_t Processor::m_img_mutex;
pthread_cond_t Processor::m_img_signal;
pthread_attr_t Processor::m_img_attr;
pthread_t *Processor::m_threads;

// CallBack from native
jobject Processor::m_jObject;
jclass Processor::m_eventClass;
jmethodID Processor::m_onCallBackID;
JavaVM *Processor::m_javaVM;