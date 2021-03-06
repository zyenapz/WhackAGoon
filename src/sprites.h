#ifndef SPRITES_H
#define SPRITES_H
#pragma once

#include <functional>
#include <utility>
#include <list>
#include <map>
#include <vector>
#include <SDL.h>
#include "g_data.h"
#include "widgets.h"

enum PosCentering;

enum BtnState
{
    BST_DISABLED,
    BST_NORMAL,
    BST_CLICKED,
    BST_HOVERED
};

class GButton
{
private:
    SDL_Rect m_rect;
    std::shared_ptr<GTexture> m_texture;
    BtnState m_state;
    std::map<BtnState, SDL_Rect> m_clips;
    std::function<void()> m_callback;

    void u_state(GameMouse *mouse_s);
    void setState(BtnState state);
    void makeCallback();

public:
    GButton(std::shared_ptr<GTexture> texture, SDL_Rect rect, std::map<BtnState, SDL_Rect> clips);
    ~GButton();
    void update(GameMouse *mouse_s);
    void draw();

    SDL_Rect getRect() { return m_rect; };
    void bindCallback(std::function<void()> cback);
};

class Particle
{
private:
    SDL_Rect m_rect;
    Vector2 m_velocity;
    SDL_Color m_color;

public:
    Particle(SDL_Rect rect, Vector2 velocity, SDL_Color color);
    ~Particle();
    void update();
    void draw();

    SDL_Point getPos();
};

class Hole
{
    friend class HoleManager;

public:
    enum HoleType
    {
        HT_NONE,
        HT_GOON,
        HT_TOWNIE,
        HT_MAYOR
    };
    enum HoleAnimState
    {
        HAS_REST,
        HAS_TOAWAKE,
        HAS_AWAKE,
        HAS_WHACKED,
        HAS_TOREST,
    };
    enum HoleHitStatus
    {
        UNSET,
        MISS,
        HIT
    };

    Hole(std::shared_ptr<GTexture> texture, SDL_Point pos, PosCentering poscenter);
    ~Hole();
    void update(GameMouse &gMouse);
    void draw();

    void awaken(HoleType type);

    HoleHitStatus fetchHitStatus() { return m_hitStat; };
    HoleType fetchHoleType();

private:
    // Spritesheet map stuff
    typedef std::pair<HoleType, HoleAnimState> HoleState;
    typedef std::map<HoleState, std::vector<SDL_Rect>> SheetMap;
    static SheetMap m_sheetMap;
    static SheetMap createSheetMap();

    // Other crap
    std::shared_ptr<GTexture> m_texture;
    SDL_Rect m_rect;
    HoleState m_state;
    int m_curF;
    HoleHitStatus m_hitStat = UNSET;

    // AnimState stuff
    const int MAX_WHACKED_LOOP = 3;
    int whacked_loop = 0;

    // Timers
    GTimer m_tmrNxtF;
    Uint32 m_delayNxtF = 80;
    static const Uint32 m_SLOW_ANIM = 400;
    static const Uint32 m_NORM_ANIM = 60;
    GTimer m_tmrAwake;
    Uint32 m_delayAwake; // TODO: Randomize later.

    // Helper methods
    void whack();
    void nextAS();
    void setHitStatus(HoleHitStatus status) { m_hitStat = status; };
    void slowFrameDelay() { m_delayNxtF = m_SLOW_ANIM; };
    void normalFrameDelay() { m_delayNxtF = m_NORM_ANIM; };
};

class PenaltyText
{
private:
    std::shared_ptr<GFont> m_font;
    std::string m_msg;
    SDL_Point m_pos;
    SDL_Color m_color;
    Vector2 m_velocity;
    Uint8 m_alpha;
    static const Uint8 ALPHA_DECREASE = 5;

public:
    PenaltyText(std::shared_ptr<GFont> font, SDL_Point pos, std::string msg);
    ~PenaltyText();

    void update();
    void draw();

    Uint8 fetchAlpha();
};

class TimerBar
{
private:
    enum TimerBar_State
    {
        DISABLED,
        ENABLED
    };
    enum FlickerState
    {
        FLICKER_SHOW,
        FLICKER_HIDE
    };
    std::shared_ptr<GTexture> m_clockTexture;
    int m_clockFrame;
    GTimer m_tmrAdvanceFrame;
    static const Uint32 FRAME_DELAY = 60;

    SDL_Rect m_rect;
    std::vector<SDL_Rect> m_clips;
    int m_origWidth;

    GTimer m_tmrDoFlicker;
    static const Uint32 FLICKER_DELAY = 150;

    GTimer m_tmrStopFlicker;
    static const Uint32 FLICKER_DURATION = FLICKER_DELAY * 10;

    TimerBar_State m_state;
    FlickerState m_flickerState;

public:
    TimerBar(std::shared_ptr<GTexture> clockTexture, CSize size, std::vector<SDL_Rect> clips);
    ~TimerBar();

    void update(Uint32 timerTicks, Uint32 delayTicks);
    void draw();

    void enable();
    void disable();

    void doFlicker();
};

class Scoreboard
{
private:
    std::shared_ptr<GFont> m_font;
    SDL_Rect m_rect;
    SDL_Color m_boardColor;
    SDL_Color m_textColor;

public:
    Scoreboard(std::shared_ptr<GFont> font, SDL_Rect rect);
    ~Scoreboard();

    void update();
    void draw(std::string score);
};

#endif // SPRITES_H