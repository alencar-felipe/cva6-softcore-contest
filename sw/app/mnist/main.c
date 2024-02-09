#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cpp_utils.h"
#include "env.h"
#include "Network.h"
#include "util.h"

void readStimulus(
    UDATA_T *inputBuffer,
    Target_T *expectedOutputBuffer)
{
    envRead(ENV_SIZE_Y * ENV_SIZE_X * ENV_NB_OUTPUTS,
            ENV_SIZE_Y, ENV_SIZE_X,
            (DATA_T *)inputBuffer, // TODO
            OUTPUTS_SIZE[0], expectedOutputBuffer);
}

int processInput(UDATA_T *inputBuffer,
                 Target_T *expectedOutputBuffer,
                 Target_T *predictedOutputBuffer,
                 UDATA_T *output_value)
{
    size_t nbPredictions = 0;
    size_t nbValidPredictions = 0;

    propagate(inputBuffer, predictedOutputBuffer, output_value);

    // assert(expectedOutputBuffer.size() == predictedOutputBuffer.size());
    for (size_t i = 0; i < OUTPUTS_SIZE[0]; i++)
    {
        if (expectedOutputBuffer[i] >= 0)
        {
            ++nbPredictions;

            if (predictedOutputBuffer[i] == expectedOutputBuffer[i])
            {
                ++nbValidPredictions;
            }
        }
    }

    return (nbPredictions > 0)
               ? nbValidPredictions
               : 0;
}

int main(int argc, char *argv[])
{

    // const N2D2::Network network{};
    size_t instret, cycles;
    size_t l1_icache_miss;
    volatile unsigned int cache_I, cache_D=0;
    volatile unsigned int Branch_miss,Branch_excp=0;
    /*
        To activate the counters, you need:
        1) To start with 0 the counter using the write_csr with the "Address Offset" on this page https://docs.openhwgroup.org/projects/cva6-user-manual/01_cva6_user/CV32A6_Control_Status_Registers.html
        2) After, you need to set the event which you want to count, with this: hpmevent[6] 0x323 [+ i*0x1]
        3) Them, you will use the read_csr with the address offset of the counter to read the value of him.
    */
    write_csr(0xB03,0x0);    // mhpmcounter3 = 0
    write_csr(0xB04,0x0);    // mhpmcounter4 = 0
    write_csr(0xB0E,0x0);    // Branch mis-predicted = 0

    write_csr(0x323,0x1);    // active the event Number of misses in L1 I-Cache
    write_csr(0x324,0x2);    // active the event Number of misses in L1 D-Cache
    write_csr(0x323,0xA);   // active the event Number of branch mispredictions



#if ENV_DATA_UNSIGNED
    UDATA_T inputBuffer[ENV_SIZE_Y * ENV_SIZE_X * ENV_NB_OUTPUTS];
#else
    std::vector<DATA_T> inputBuffer(network.inputSize(a));
#endif

    Target_T expectedOutputBuffer[OUTPUTS_SIZE[0]];
    Target_T predictedOutputBuffer[OUTPUTS_SIZE[0]];
    UDATA_T output_value;

    readStimulus(inputBuffer, expectedOutputBuffer);
    instret = -read_csr(minstret);
    cycles = -read_csr(mcycle);
    //l1_icache_miss = -read_csr(ml1_icache_miss);
    cache_I = read_csr(0xB03);
    cache_D = read_csr(0xB04);
    Branch_miss = read_csr(0xB0E);

    const int success = processInput(inputBuffer,
                                     expectedOutputBuffer,
                                     predictedOutputBuffer,
                                     &output_value);

    instret += read_csr(minstret);
    cycles += read_csr(mcycle);
    cache_I += read_csr(0xB03);
    cache_D += read_csr(0xB04);

    l1_icache_miss += -read_csr(ml1_icache_miss);
    Branch_miss += read_csr(0xB0E);

    printf("Expected  = %d\n", expectedOutputBuffer[0]);
    printf("Predicted = %d\n", predictedOutputBuffer[0]);
    printf("Result : %d/1\n", success);
    printf("credence: %d\n", output_value);
    printf("image %s: %d instructions\n", stringify(MNIST_INPUT_IMAGE), (int)(instret));
    printf("image %s: %d cycles\n", stringify(MNIST_INPUT_IMAGE), (int)(cycles));
    printf("Cache Miss Instructions = [%d]\nCache Miss Data = [%d]\n", cache_I, cache_D);  // instr = 184 data = 9400 
    printf("---------------------------\nITLB = [%d]\n",(int)(l1_icache_miss));
    
    //printf("---------------------------\nITLB = [%d]\nDTLB = [%d]\n",ITLB,DTLB);
    printf("---------------------------\nBranch Miss Predictions = [%d]\n",Branch_miss);
    printf("---------------------------\n");
    
#ifdef OUTPUTFILE
    FILE *f = fopen("success_rate.txt", "w");
    if (f == NULL)
    {
        N2D2_THROW_OR_ABORT(std::runtime_error,
                            "Could not create file:  success_rate.txt");
    }
    fprintf(f, "%f", successRate);
    fclose(f);
#endif
}

// counter branch mispredicsts
// miss rates
