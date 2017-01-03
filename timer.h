#ifndef TIMER_H
#define TIMER_H

//#include <SDL.h>

struct Timer
{
private:
	bool paused{ false };
	bool started{ false };
	Uint32 start_t{ 0 };
	Uint32 paused_t{ 0 };

	//Variables necessary for getting the averages
	int update_threshold{ 30 };   //default count after how many frames the average is calculated
	double avg_fps{ 0 };
	double avg_fps_sum{ 0 };
	unsigned int frame_counter{ 0 };
public:
	Timer() {};

	void start();
	void stop();
	void pause();
	void unpause();
	void update_avg_fps();

	//Getter functions
	bool is_started()       const noexcept;
	bool is_paused()        const noexcept;
	double get_fps()        const noexcept;
	double get_average_fps()const noexcept;
	Uint32 get_time()       const noexcept;  //Returns the time since the timer started
};

#endif // TIMER_H
