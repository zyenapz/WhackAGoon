#include <iostream>
#include <memory>
#include <functional>
#include <cassert>
#include <SDL.h>
#include "scenes.h"
#include "widgets.h"
#include "g_data.h"

#pragma region Scene
void Scene::checkTransition()
{
    switch (m_stflag)
    {
    case STF_TOPLAY:
        m_context->transitionTo(new PlayScene());
        break;
    case STF_TOMENU:
        m_context->transitionTo(new MenuScene());
    default:
        break;
    }

    m_stflag = STF_NONE;
}
#pragma endregion Scene

#pragma region MenuScene
MenuScene::MenuScene()
{
    resetMouseState();
    loadAssets();
    createButtons();
}
MenuScene::~MenuScene()
{
    //delete brickBGTexture;
    //delete uiElementsTexture;

    //delete btnPlay;
    //delete btnHelp;
    //delete btnInfo;
    printf("MenuScene deleted.\n");
}
void MenuScene::handleEvents(SDL_Event *e)
{
    // Reset mouse
    m_gMouse.isClicked = false;

    while (SDL_PollEvent(e))
    {
        if (e->type == SDL_QUIT)
        {
            m_context->setQuitFlag(true);
        }
        if (e->type == SDL_MOUSEBUTTONDOWN)
        {
            m_gMouse.isClicked = true;
        }
        if (e->type == SDL_MOUSEBUTTONUP)
        {
            m_gMouse.isClicked = false;
        }
        if (e->type == SDL_KEYDOWN)
        {
            switch (e->key.keysym.sym)
            {
            case SDLK_F1:
                m_stflag = STF_TOMENU;
                break;
            case SDLK_F2:
                m_stflag = STF_TOPLAY;
                break;
            }
        }
    }

    // Get mouse state
    SDL_GetMouseState(&m_gMouse.position.x, &m_gMouse.position.y);
}
void MenuScene::update()
{
    btnPlay->update(&m_gMouse);
    btnHelp->update(&m_gMouse);
    btnInfo->update(&m_gMouse);
}
void MenuScene::draw()
{
    // Rects
    WindowMetadata winData;
    SDL_Rect logoRect = {50, 50, 80 * winData.PXSCALE, 50 * winData.PXSCALE};

    SDL_SetRenderDrawColor(gameRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gameRenderer);

    brickBGTexture->draw();
    logoTexture->draw(NULL, &logoRect);
    btnPlay->draw();
    btnHelp->draw();
    btnInfo->draw();

    SDL_RenderPresent(gameRenderer);
};
void MenuScene::loadAssets()
{
    // Images
    uiElementsTexture = std::unique_ptr<GTexture>(new GTexture());
    uiElementsTexture->loadFromFile("assets/images/ui_elements.png");

    brickBGTexture = std::unique_ptr<GTexture>(new GTexture());
    brickBGTexture->loadFromFile("assets/images/brick_bg.png");

    logoTexture = std::unique_ptr<GTexture>(new GTexture());
    logoTexture->loadFromFile("assets/images/logo.png");

    // Sounds
    // --- TODO ---
}
void MenuScene::createButtons()
{
    WindowMetadata winData;

    // Anchor points
    int btnW = 16 * winData.PXSCALE;
    int btnH = 16 * winData.PXSCALE;
    int cx = (winData.WIDTH / 2) - (btnW / 2);

    // --- btnPlay --
    SDL_Rect btnPlay_rect = {cx, static_cast<int>(winData.HEIGHT * 0.70), btnW, btnH};
    std::map<BtnState, SDL_Rect> btnPlay_clips;

    btnPlay_clips[BST_NORMAL] = {0, 24, 16, 16};
    btnPlay_clips[BST_HOVERED] = {16, 24, 16, 16};
    btnPlay = std::make_unique<GButton>(GButton(uiElementsTexture.get(), btnPlay_rect, btnPlay_clips));
    btnPlay->bindCallback(std::bind(&MenuScene::cbPlay, this));

    // --- btnHelp ---
    SDL_Rect btnHelp_rect = {static_cast<int>(cx - (btnW * 1.5)), static_cast<int>(winData.HEIGHT * 0.75), btnW, btnH};
    std::map<BtnState, SDL_Rect> btnHelp_clips;
    btnHelp_clips[BST_NORMAL] = {32, 24, 16, 16};
    btnHelp_clips[BST_HOVERED] = {48, 24, 16, 16};
    btnHelp = std::make_unique<GButton>(GButton(uiElementsTexture.get(), btnHelp_rect, btnHelp_clips));
    btnHelp->bindCallback(std::bind(&MenuScene::cbHelp, this));

    // --- btnInfo ---
    SDL_Rect btnInfo_rect = {static_cast<int>(cx + (btnW * 1.5)), static_cast<int>(winData.HEIGHT * 0.75), btnW, btnH};
    std::map<BtnState, SDL_Rect> btnInfo_clips;
    btnInfo_clips[BST_NORMAL] = {0, 56, 16, 16};
    btnInfo_clips[BST_HOVERED] = {16, 56, 16, 16};
    btnInfo = std::make_unique<GButton>(GButton(uiElementsTexture.get(), btnInfo_rect, btnInfo_clips));
    btnInfo->bindCallback(std::bind(&MenuScene::cbInfo, this));
}
// *** CALLBACKS ***
void MenuScene::cbPlay()
{
    m_stflag = STF_TOPLAY;
}
void MenuScene::cbHelp()
{
    // --- TODO ---
}
void MenuScene::cbInfo()
{
    // --- TODO ---
}
#pragma endregion MenuScene

#pragma region PlayScene
PlayScene::PlayScene()
{
    resetMouseState();
    loadAssets();
    createButtons();
    initializeTimers();
}
PlayScene::~PlayScene()
{
}
void PlayScene::handleEvents(SDL_Event *e)
{
    // Reset mouse
    m_gMouse.isClicked = false;

    while (SDL_PollEvent(e))
    {
        if (e->type == SDL_QUIT)
        {
            m_context->setQuitFlag(true);
        }
        if (e->type == SDL_MOUSEBUTTONDOWN)
        {
            m_gMouse.isClicked = true;
        }
        if (e->type == SDL_MOUSEBUTTONUP)
        {
            m_gMouse.isClicked = false;
        }
        if (e->type == SDL_KEYDOWN)
        {
            switch (e->key.keysym.sym)
            {
            case SDLK_F1:
                m_stflag = STF_TOMENU;
                break;
            case SDLK_F2:
                m_stflag = STF_TOPLAY;
                break;
            }
        }
    }

    SDL_GetMouseState(&m_gMouse.position.x, &m_gMouse.position.y);
}
void PlayScene::update()
{
    btnToMenu->update(&m_gMouse);

    // Update timers
    if (m_tmrWarmup.getTicks() >= m_delayWarmup)
    {
        printf("Warmup is overz!!\n");
    }
}
void PlayScene::draw()
{
    GameColors gColors;
    WindowMetadata winDat;

    SDL_SetRenderDrawColor(gameRenderer, 200, 200, 0, 255);
    SDL_RenderClear(gameRenderer);

    auraBGTexture->draw();
    cityBGTexture->draw();
    btnToMenu->draw();
    m_gFontInfo->draw("Hello!", {winDat.HEIGHT / 2, 50}, gColors.WHITE, PosCentering::POSCEN_X);

    SDL_RenderPresent(gameRenderer);
}
void PlayScene::loadAssets()
{
    // Load images
    cityBGTexture = std::unique_ptr<GTexture>(new GTexture());
    cityBGTexture->loadFromFile("assets/images/city_bg.png");

    auraBGTexture = std::unique_ptr<GTexture>(new GTexture());
    auraBGTexture->loadFromFile("assets/images/aura_bg.png");

    uiElementsTexture = std::unique_ptr<GTexture>(new GTexture());
    uiElementsTexture->loadFromFile("assets/images/ui_elements.png");

    // Load fonts
    m_gFontInfo = std::unique_ptr<GFont>(new GFont());
    m_gFontInfo->loadFontFromFile("assets/fonts/04B03.TTF", 48);
}
void PlayScene::createButtons()
{
    WindowMetadata winData;

    // --- btnToMenu --
    const int btnToMenu_rw = 8 * winData.PXSCALE;
    const int btnToMenu_rh = 8 * winData.PXSCALE;
    const int btnToMenu_rx = (winData.WIDTH - winData.PXSCALE) - btnToMenu_rw;
    const int btnToMenu_ry = winData.PXSCALE;
    SDL_Rect btnToMenu_rect = {btnToMenu_rx, btnToMenu_ry, btnToMenu_rw, btnToMenu_rh};
    std::map<BtnState, SDL_Rect> btnToMenu_clips;
    btnToMenu_clips[BST_NORMAL] = {0, 16, 8, 8};
    btnToMenu_clips[BST_HOVERED] = {8, 16, 8, 8};
    btnToMenu = std::make_unique<GButton>(GButton(uiElementsTexture.get(), btnToMenu_rect, btnToMenu_clips));
    btnToMenu->bindCallback(std::bind(&PlayScene::cbToMenu, this));
}
void PlayScene::initializeTimers()
{
    m_tmrWarmup.start();
}
// *** CALLBACKS ***
void PlayScene::cbToMenu()
{
    m_stflag = STF_TOMENU;
}
#pragma endregion PlayScene