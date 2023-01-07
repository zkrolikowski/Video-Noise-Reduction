import ffmpeg

def use_anlmdn():
    ## open source video file
    inFile = ffmpeg.input('./TestFiles/inFileNoise.mp4')

    # add filter
    #strenght 0.00001 to 10000, patch, 1 to 100ms, research 2 to 300ms
    audio2 = inFile.audio.filter('anlmdn', '0.0003', '0.010', '0.012')
    video = inFile.video

    # render output
    out = ffmpeg.output(audio2, video,'.\\TestFiles\\outFileNoNoise_anlmdn.mp4', acodec='aac').overwrite_output()
    out.run()

def use_afftdn():
    ## open source video file
    inFile = ffmpeg.input('./TestFiles/inFileNoise.mp4')

    # add filter
    audio2 = inFile.audio.filter('afftdn', '6')
    video = inFile.video

    # render output
    out = ffmpeg.output(audio2, video,'.\\TestFiles\\outFileNoNoise_afftdn.mp4', acodec='aac').overwrite_output()
    out.run()

def use_arnndn():
    ## open source video file
    inFile = ffmpeg.input('./TestFiles/inFileNoise.mp4')

    # add filter
    audio2 = inFile.audio.filter('arnndn', '.\\rnnoise-models\\marathon-prescription-2018-08-29\\mp.rnnn' )
    video = inFile.video

    # render output
    out = ffmpeg.output(audio2, video,'.\\TestFiles\\outFileNoNoise_arnndn.mp4', acodec='aac').overwrite_output()
    out.run()

'''   
## convert audio to raw pcm
    out, _ = (inFile.audio
    .output('./TestFiles/tempAudio.wav', acodec='pcm_s16le', ac=1, ar='48k')
    .overwrite_output()
    .run(capture_stdout=True)) 
'''

def main():

    inFile = ffmpeg.input('./TestFiles/inFileNoise.mp4')
    ## convert audio to raw pcm
    out, _ = (inFile.audio
    .output('./TestFiles/tempAudio.wav', acodec='pcm_s16le', ac=1, ar='48k', map_metadata=-1)
    .overwrite_output()
    .run(capture_stdout=True)) 


    #use_anlmdn()
    #use_afftdn()
    #use_arnndn() 
   

if __name__ == "__main__":
    main()