/*
#include <SDL.h>
#include <time.h>
#include "timer.h"
*/

void Timer::start()
{
    //Start the timer
    started = true;
    paused = false;

    start_t = SDL_GetTicks();
    paused_t = 0;
}

void Timer::stop()
{
    //Stop the timer
    started = false;

    //Unpause the timer
    paused = false;

    //Clear the variables
    start_t = 0;
    paused_t = 0;
}

void Timer::pause()
{
    //If the timer is running and is not paused
    if(started && !paused)
    {
        paused = true;
        paused_t = SDL_GetTicks();

        start_t = 0;
    }
}

void Timer::unpause()
{
    if(started && paused)
    {
        paused = false;

        start_t = SDL_GetTicks();
        paused_t = 0;
    }
}

void Timer::update_avg_fps()
{
    ++frame_counter;
    if(frame_counter%update_threshold == 0)
    {
        avg_fps = avg_fps_sum / update_threshold;
        frame_counter = 0;
        avg_fps_sum = 0;
    }
    else
    {
        avg_fps_sum += get_fps();
    }
}

double Timer::get_fps() const noexcept
{
    return 1000.0/get_time();
}

double Timer::get_average_fps() const noexcept
{
    return avg_fps;
}

bool Timer::is_started()    const noexcept
{
    return started;
}

bool Timer::is_paused()     const noexcept
{
    return paused;
}
Uint32 Timer::get_time()    const noexcept
{
    Uint32 time = 0;
    if(started)
    {
        //Returns the time when paused
        if(paused)
        {
            time =  paused_t;
        }
        //Returns the time SINCE the timer started
        else
        {
            time = SDL_GetTicks() - start_t;
        }
    }
    return time;
}
