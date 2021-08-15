#include<iostream>
#include<fstream>
#include<pthread.h>
#include<unistd.h>
#include<vector>
#include<random>
#include<chrono>
#include<thread>
#include <atomic>




/*-------------------------------------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------Filter Lock class & function definitions begin--------------------------------------*/



// Parameters n,k,lambda1 and lambda2 appear in different form in this code. For e.g. n may appear in form of n,n1,n2,n3 or n4. n=n1=n2=n3=n4.
// They just same value but different name because they are defined in different scopes. Same is with k,k1,k2,k3 & k4,
// lambda1,lambda12, lambda13 & lambda14 and lambda2,lambda22, lambda23 & lambda24



class CLHLock
{

public:

    std::atomic<std::atomic<bool>*> tail;           //Atomic pointer to atomic bool



    CLHLock()
    {
        tail = new std::atomic_bool;
        (*tail).store(false);

    }

    ~CLHLock()
    {
        delete tail;
    }


    void lock(std::atomic<std::atomic<bool>*>* ptr_to_myPred)
    {
        (**ptr_to_myPred).store(true);
        *ptr_to_myPred = tail.exchange(*ptr_to_myPred);
        while((**ptr_to_myPred).load()==true)
        {
            ;
        }

    }

    void unlock(std::atomic<std::atomic<bool>*>* ptr_to_myPred, std::atomic<std::atomic<bool>*>* ptr_to_myNode)
    {
        (**ptr_to_myNode).store(false);
        *ptr_to_myNode = (*ptr_to_myPred).load();
    }

};





class CLH_PARAMETERS                                 //Class to be used to pass parameters to the test threads
{
    public:
        int thread_id3;
        int k3;
        double lambda31;
        double lambda32;
        CLHLock* cptr3;
        int* timing_ptr;
        int n3;
};




void* testCS_clhlock(void* arg)
{

    std::atomic<std::atomic<bool>*> myPred;
    std::atomic<std::atomic<bool>*> myNode;
    myPred = new std::atomic_bool;
    myNode= myPred.load();





    int thread_id2 = (static_cast<CLH_PARAMETERS*>(arg))->thread_id3;
    int k2 = (static_cast<CLH_PARAMETERS*>(arg))->k3;
    double lambda21 = (static_cast<CLH_PARAMETERS*>(arg))->lambda31;
    double lambda22 = (static_cast<CLH_PARAMETERS*>(arg))->lambda32;
    CLHLock* cptr = (static_cast<CLH_PARAMETERS*>(arg))->cptr3;
    int* subarr_F=(static_cast<CLH_PARAMETERS*>(arg))->timing_ptr;



    std::default_random_engine generator1;
    std::exponential_distribution<double> distribution1(1/lambda21);      //In the assignment Lambdas are mentioned to be have units of time (in sec)

    std::default_random_engine generator2;
    std::exponential_distribution<double> distribution2(1/lambda22);      //In the assignment Lambdas are mentioned to be have units of time (in sec)

    std::ofstream outputfile("Output.txt",std::ofstream::app);
    if (!outputfile.is_open())
    {
        std::cout << "Unable to open file";
        return nullptr;
    }



    for (int j=0;j<k2;j++)                          //Each thread loops for 'k2' times
    {
        unsigned long long int reqtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        reqtime = ( reqtime + 19800000)%86400000;                   //A day consists of 86400000 milliseconds. 19800000 added to adjust international time delay.

//        std::cout<<j+1<< " CS Entry Request at "<<reqtime/3600000<<" hours "<<reqtime%3600000/60000<<" min "
//              <<reqtime%60000/1000<<" sec "<< reqtime%1000<<" millisec by thread "<<thread_id2<<std::endl;

//        outputfile<<j+1<< " CS Entry Request at "<<reqtime/3600000<<" hours "<<reqtime%3600000/60000<<" min "
//              <<reqtime%60000/1000<<" sec "<< reqtime%1000<<" millisec by thread "<<thread_id2<<std::endl;
        subarr_F[4*(j)]=reqtime;


        cptr->lock(&myPred);                 // Call for lock


        unsigned long long int enttime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        enttime = ( enttime + 19800000)%86400000;                   //A day consists of 86400000 milliseconds. 19800000 added to adjust international time delay.

        std::cout<<j+1<< " CS Entry at "<<enttime/3600000<<" hours "<<enttime%3600000/60000<<" min "
                <<enttime%60000/1000<<" sec "<< enttime%1000<<" millisec by thread "<<thread_id2<<std::endl;
        outputfile<<j+1<< " CS Entry at "<<enttime/3600000<<" hours "<<enttime%3600000/60000<<" min "
                <<enttime%60000/1000<<" sec "<< enttime%1000<<" millisec by thread "<<thread_id2<<std::endl;
        subarr_F[4*(j)+1]=enttime;




        double T1= distribution1(generator1);
        int t1 = (int)1000000.0*T1;
        std::this_thread::sleep_for(std::chrono::microseconds(t1));         //Sleep for t1 microseconds



        unsigned long long int reqexittime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        reqexittime = ( reqexittime + 19800000)%86400000;           //A day consists of 86400000 milliseconds. 19800000 added to adjust international time delay.

        std::cout<<j+1<< " CS Exit Request at "<<reqexittime/3600000<<" hours "<<reqexittime%3600000/60000<<" min "
                <<reqexittime%60000/1000<<" sec "<< reqexittime%1000<<" millisec by thread "<<thread_id2<<std::endl;
        outputfile<<j+1<< " CS Exit Request at "<<reqexittime/3600000<<" hours "<<reqexittime%3600000/60000<<" min "
                <<reqexittime%60000/1000<<" sec "<< reqexittime%1000<<" millisec by thread "<<thread_id2<<std::endl;
        subarr_F[4*(j)+2]=reqexittime;



        cptr->unlock(&myPred, &myNode);                                   // Call for unlock



        unsigned long long int exittime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        exittime = ( exittime + 19800000)%86400000;                 //A day consists of 86400000 milliseconds. 19800000 added to adjust international time delay.

//        std::cout<<j+1<< " CS Exit at "<<exittime/3600000<<" hours "<<exittime%3600000/60000<<" min "
//              <<exittime%60000/1000<<" sec "<< exittime%1000<<" millisec by thread "<<thread_id2<<std::endl;
//        outputfile<<j+1<< " CS Exit at "<<exittime/3600000<<" hours "<<exittime%3600000/60000<<" min "
//              <<exittime%60000/1000<<" sec "<< exittime%1000<<" millisec by thread "<<thread_id2<<std::endl;
        subarr_F[4*(j)+3]=exittime;


        double T2= distribution2(generator2);
        int t2 = (int)1000000.0*T2;
        std::this_thread::sleep_for(std::chrono::microseconds(t2));     //Sleep for t2 microseconds


    }

    std::this_thread::sleep_for(std::chrono::seconds(5));


    delete myPred;

    outputfile.close();

    return nullptr;
}

/*---------------------------------------Filter Lock class & function definitions end--------------------------------------*/


/*-------------------------------------------------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------Main Code begins----------------------------------------------------*/


int main()
{


    int n1, k1;
    double lambda11, lambda12;
    std::ifstream input_file("inp-params.txt");
    if (!input_file.is_open())
    {
        std::cout << "Unable to open file";
        return 0;
    }
    input_file>>n1;
    input_file>>k1;
    input_file>>lambda11;
    input_file>>lambda12;
    input_file.close();

    std::ofstream outputfile("Output.txt",std::ofstream::trunc);
    if (!outputfile.is_open())
    {
        std::cout << "Unable to open file";
        return 1;
    }


/*--------------------------------------------------Common Code ends---------------------------------------------------*/



/*-------------------------------------------------------------------------------------------------------------------------------------------------*/



/*-------------------------------------------------Filter Lock begins--------------------------------------------------*/



    CLHLock* clhlock;
    clhlock = new CLHLock;


    int arr_F[n1][k1*4];

    CLH_PARAMETERS clh_parameters[n1];

    for (int j=0;j<n1;j++)
    {
        clh_parameters[j].thread_id3=j;
        clh_parameters[j].k3=k1;
        clh_parameters[j].lambda31=lambda11;
        clh_parameters[j].lambda32=lambda12;
        clh_parameters[j].cptr3=clhlock;
        clh_parameters[j].timing_ptr=(int*)arr_F[j];
        clh_parameters[j].n3=n1;
    }

    pthread_t test_threads_clh[n1];

    for (int j=0;j<n1;j++)
    {
        int code = pthread_create(&test_threads_clh[j],NULL,testCS_clhlock, (void*)&clh_parameters[j]);
        if (code)
        {
            std::cout << "Error in creating thread"<< std::endl;
            return 1;
        }
    }

    for (int j=0;j<n1;j++)
    {
        pthread_join(test_threads_clh[j],NULL);
    }



    FILE * pFile;

    pFile = fopen ("outputfile_CLH.txt","w");                //Data to plot graphs

    for (int j=0 ; j<n1 ; j++)
    {
     for (int l=0 ; l<k1 ; l++)
     {
         fprintf (pFile, "%d,",arr_F[j][4*l+1]-arr_F[j][4*l]);
         fprintf (pFile, "%d\n",arr_F[j][4*l+3]-arr_F[j][4*l+2]);
     }
    }

    fprintf (pFile, "\n\n\n");

    fclose (pFile);




    delete clhlock;


    outputfile.close();

    return 0;


/*-------------------------------------------------Filter Lock ends--------------------------------------------------*/


/*-------------------------------------------------------------------------------------------------------------------------------------------------*/


}



