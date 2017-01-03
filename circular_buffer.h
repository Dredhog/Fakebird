struct playback_buffer {
	game_input *Inputs;
	void *States;

	uint32 InputStart;
	uint32 InputEnd;
	uint32 InputCapacity;
	uint32 InputCount;
	uint32 InputsPerState;

	uint32 PlaybackStart;
	uint32 PlaybackIndex;
	uint32 PlaybackCount;

	uint32 StateStart;
	uint32 StateEnd;
	uint32 StateSizeInBytes;
	uint32 StateCapacity;
	uint32 StateCount;
};


playback_buffer NewPlaybackBuffer(uint32 StateCapacity, uint32 InputsPerState, uint32 StateSizeInBytes)
{
	assert(StateCapacity);
	assert(StateSizeInBytes);
	assert(InputsPerState);

	playback_buffer Result = {};
	Result.StateSizeInBytes = StateSizeInBytes;
	Result.StateCapacity = StateCapacity;
	Result.InputsPerState = InputsPerState;
	Result.InputCapacity = StateCapacity*InputsPerState;
	Result.States = malloc(StateCapacity*StateSizeInBytes);
	Result.Inputs = (game_input*)malloc(Result.InputCapacity*sizeof(game_input));
	return Result;
}

bool32 StatesFull(playback_buffer *Buffer) {
	return Buffer->StateCount == Buffer->StateCapacity;
}

bool32 InputsFull(playback_buffer *Buffer) {
	return Buffer->InputCount == Buffer->InputCapacity;
}

void PushPlaybackBuffer(playback_buffer *B, game_input *GameInput, void *GameMemory, uint32 Frame) {
	assert(GameInput);

	//push input
	memcpy(B->Inputs + B->InputEnd, GameInput, sizeof(game_input));
	B->InputEnd++;
	B->InputEnd %= B->InputCapacity;

	if (InputsFull(B)) {
		B->InputStart = (B->InputStart + 1) % B->InputCapacity;
	}
	else {
		B->InputCount++;
	}

	//ocasionally push state
	if (Frame % B->InputsPerState == 0) {
		assert(GameMemory);
		memcpy((char*)B->States + (B->StateEnd * B->StateSizeInBytes), GameMemory, B->StateSizeInBytes);
		B->StateEnd++;
		B->StateEnd %= B->StateCapacity;

		if (StatesFull(B)) {
			B->StateStart = (B->StateStart + 1) % B->StateCapacity;
		}
		else {
			B->StateCount++;
		}
	}

	B->PlaybackIndex = 0;
}

void PeekAndStepPlaybackBuffer(playback_buffer *B, game_input *DestInput, void *DestMemory, uint32 Frame) {
	assert(DestInput);
	assert(B->InputCount);

	if (StatesFull(B)) {
		B->PlaybackStart = B->StateStart * B->InputsPerState;
		B->PlaybackCount = B->InputCount - B->InputCount % B->InputsPerState;
	}
	else {
		B->PlaybackStart = 0;
		B->PlaybackCount = B->InputCount;
	}

	uint32 InputIndex = (B->PlaybackStart + B->PlaybackIndex) % B->InputCapacity;
	memcpy(DestInput, B->Inputs + InputIndex, sizeof(game_input));

	if (B->PlaybackIndex == 0) {
		assert(B->StateCount > 0);
		assert(DestMemory);
		memcpy(DestMemory, (char*)B->States + (B->StateStart * B->StateSizeInBytes), B->StateSizeInBytes);
	}

	B->PlaybackIndex++;
	B->PlaybackIndex %= B->PlaybackCount;
}

void
ClearPlaybackBuffer(playback_buffer *B) {
	B->InputStart = 0;
	B->InputEnd = 0;
	B->InputCount = 0;

	B->PlaybackStart = 0;
	B->PlaybackIndex = 0;
	B->PlaybackCount = 0;

	B->StateStart = 0;
	B->StateEnd = 0;
	B->StateCount = 0;
}

internal void
DestroyPlaybackBuffer(playback_buffer *Buffer)
{
	assert(Buffer->Inputs);
	assert(Buffer->States);
	free(Buffer->Inputs);
	free(Buffer->States);
}
