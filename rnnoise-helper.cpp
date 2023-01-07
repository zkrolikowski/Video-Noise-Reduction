#include "rnnoise/include/rnnoise.h"
#include <fstream>
#include <iostream>
#include <memory>

constexpr int blockSize = 480;

/*!
 * Generic and Data headers are the same header
 */
struct GenericHeaderChunk
{
    char label[4];      // label for given chunk
    unsigned chunkSize; // size of given chunk
};

/*!
 * Header for a riff file
 */
struct RiffHeader
{
    char riff_label[4]; // (00) = {'R','I','F','F'}
    unsigned riff_size; // (04) = 36 + data_size
    char file_tag[4]; // (08) = {'W','A','V','E'}
};

/*!
 * Format chunk
 */
struct FormatHeader
{
    char label[4]; // (12) = {'f','m','t',' '}
    unsigned size; // (16) = 16
    unsigned short audio_format; // (20) = 1
    unsigned short channel_count; // (22) = 1 or 2
    unsigned sampling_rate; // (24)
    unsigned bytes_per_second; // (28)
    unsigned short bytes_per_sample; //
    unsigned short bits_per_sample; // (34) = 8 or 16
};

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cout << "Usage" << std::endl;
    }

    std::fstream inFile(argv[1], std::ios_base::binary | std::ios_base::in);
    std::fstream outFile(argv[2], std::ios_base::binary | std::ios_base::out);

    if(!inFile || !outFile)
    {
        std::cout << "invalid source file" << std::endl;
        return 0;
    }

    RiffHeader riffHeader;
    inFile.read(reinterpret_cast<char*>(&riffHeader), sizeof(RiffHeader));

    FormatHeader formatHeader;
    inFile.read(reinterpret_cast<char*>(&formatHeader), sizeof(FormatHeader));

    GenericHeaderChunk dataChunk;
    inFile.read(reinterpret_cast<char*>(&dataChunk), sizeof(GenericHeaderChunk));
    while(dataChunk.label[0] != 'd' && 
          dataChunk.label[1] != 'a' && 
          dataChunk.label[2] != 't' && 
          dataChunk.label[3] != 'a')
    {
        inFile.seekg(dataChunk.chunkSize, std::ios_base::cur);
        inFile.read(reinterpret_cast<char*>(&dataChunk), sizeof(GenericHeaderChunk));
    }

    if(formatHeader.audio_format != 1 || formatHeader.channel_count != 1 || formatHeader.sampling_rate != 48000)
    {
        std::cout << "invaild format" << std::endl;
        return 0;   
    }
    
    short* rawData = new short[dataChunk.chunkSize]();
    float* processData = new float[dataChunk.chunkSize]();

    inFile.read(reinterpret_cast<char*>(rawData), dataChunk.chunkSize);

    for(int i = 0; i < dataChunk.chunkSize / 2; ++i)
    {
        processData[i] = rawData[i] * 2;
    }

    DenoiseState* denoiseState;
    denoiseState = rnnoise_create(nullptr);
    for(int i = 0; i < dataChunk.chunkSize; i += blockSize)
    {    
        rnnoise_process_frame(denoiseState, reinterpret_cast<float*>(processData + i), reinterpret_cast<float*>(processData + i));
    }
    rnnoise_destroy(denoiseState);

    for(int i = 0; i < dataChunk.chunkSize / 2; ++i)
    {
        rawData[i] = static_cast<short>(processData[i]);
    }

    outFile.write(reinterpret_cast<char*>(&riffHeader), sizeof(RiffHeader));
    outFile.write(reinterpret_cast<char*>(&formatHeader), sizeof(FormatHeader));
    outFile.write(reinterpret_cast<char*>(&dataChunk), sizeof(GenericHeaderChunk));
    outFile.write(reinterpret_cast<char*>(rawData), dataChunk.chunkSize);

    delete [] rawData;
    delete [] processData;

    return 0;
}