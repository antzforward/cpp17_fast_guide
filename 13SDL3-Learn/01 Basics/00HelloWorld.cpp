// 00HelloWorld.cpp
#define SDL_MAIN_USE_CALLBACKS 1 // 启用 SDL3 最新的回调架构，替代传统 main()
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// 你自己的应用状态，用 C++ 的 struct 组织
struct AppState {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};

// 1. 初始化（启动时调用 1 次）
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    AppState* state = new AppState();
    *appstate = state;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_APP_FAILURE;
    }

    // SDL3 创建窗口和渲染器的最新 API
    if (!SDL_CreateWindowAndRenderer("SDL3 Hello World", 800, 600, 0, &state->window, &state->renderer)) {
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE; // 成功，继续运行
}

// 2. 事件处理（操作系统有键盘鼠标消息时自动触发）
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; // 收到点 X 关闭窗口的事件，返回 SUCCESS 退出程序
    }
    return SDL_APP_CONTINUE; 
}

// 3. 渲染循环（每一帧自动调用 1 次）
SDL_AppResult SDL_AppIterate(void* appstate) {
    AppState* state = static_cast<AppState*>(appstate);

    // 刷一个绿色的背景
    SDL_SetRenderDrawColor(state->renderer, 0, 150, 0, 255);
    SDL_RenderClear(state->renderer);
    SDL_RenderPresent(state->renderer);

    return SDL_APP_CONTINUE; 
}

// 4. 清理资源（程序结束前自动调用）
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    if (appstate) {
        AppState* state = static_cast<AppState*>(appstate);
        SDL_DestroyRenderer(state->renderer);
        SDL_DestroyWindow(state->window);
        delete state;
    }
}

//cl /EHsc /std:c++23 00HelloWorld.cpp /link SDL3.lib /SUBSYSTEM:CONSOLE
