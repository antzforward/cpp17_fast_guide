#include <SDL2/SDL.h>
#include <cmath>

//编译指令真麻烦，还没有成功 2024年2月28日
const int AMPLITUDE = 28000;
const int FREQUENCY = 44100;

void audio_callback(void* userdata, Uint8* stream, int len) {
    static double audio_position = 0;
    static double phase_increment = 2 * M_PI * 440 / FREQUENCY; // 440Hz tone
    Sint16* buffer = (Sint16*)stream;
    int length = len / 2; // 2 bytes per sample for AUDIO_S16SYS
    for(int i = 0; i < length; i++, audio_position += phase_increment) {
        double sine_wave = sin(audio_position);
        buffer[i] = (Sint16)(sine_wave * AMPLITUDE);
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec desiredSpec;
    desiredSpec.freq = FREQUENCY;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = audio_callback;
    desiredSpec.userdata = NULL;

    SDL_AudioSpec obtainedSpec;
    // you might want to check for errors here
    SDL_OpenAudio(&desiredSpec, &obtainedSpec);

    // start play audio
    SDL_PauseAudio(0);

    // let it play for 5 seconds
    SDL_Delay(5000);

    SDL_CloseAudio();
    SDL_Quit();
    
    return 0;
}