#include "anim.h"
#include "util.h"

float lerp(float t0, float v0, float t1, float v1, float t)
{
	float v;

	/* Linear interpolation, or lerp. */
	v = v0 + (t - t0) * (v1 - v0) / (t1 - t0);

	return v;
}

int findInterval(KeyFrame kf[], int n, float t)
{
	int i;

	/* Use linear search (better approaches possible). */
	for (i = n-2; i >=0; i--) {
		if (t >= kf[i].time)
			break;
	}

	return i;
}


/* Callback for animation. */
bool animate(float elapsedTime, Interpolator interpolator, float * rot)
{
	// elapsedTime in second
	float elapsedTimeAnimation;
	int i;

	elapsedTimeAnimation = elapsedTime - interpolator.startTime;

	/* Keyframe calculations */
	if (elapsedTimeAnimation >= 
			interpolator.keyFrames[interpolator.nKeyFrames-1].time) {
		/* Finished. */
		i = interpolator.nKeyFrames;
		return false;
	}
	else {
	/* Find key frames to interpolate. */
	i = findInterval(interpolator.keyFrames,
			interpolator.nKeyFrames,
			elapsedTimeAnimation);
	}

	/* Use linear interpolation to work out intermediate value. */
	if (i >= interpolator.nKeyFrames) 
		* rot = interpolator.keyFrames[interpolator.nKeyFrames-1].value;
	else
		* rot = lerp(interpolator.keyFrames[i].time,
			interpolator.keyFrames[i].value,
			interpolator.keyFrames[i+1].time,
			interpolator.keyFrames[i+1].value,
			elapsedTimeAnimation);
	return true;
}