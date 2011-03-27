/*
 * AnimationSequence.h
 *
 *  Created on: 2010-03-14
 *      Author: Mero
 */

#ifndef ANIMATIONSEQUENCE_H_
#define ANIMATIONSEQUENCE_H_

//! Interpolate
template <typename T>
inline T LERP(const T &valA, const T &valB, float fraction) {
	return valA + fraction * (valB - valA);
}

//! if you want to loop then a->setNext(a); loop only with root animation
//! animation will be deleted after it reaches end ( next = null )
class AnimationSequence
{
public:
	AnimationSequence();
	virtual ~AnimationSequence();

	//! set next animation
	void setNext(AnimationSequence* next);

	//! reset animation
	virtual void reset();

	//! register in system
	static void add(AnimationSequence* root);

	//! unregister from system
	static void remove(AnimationSequence* root);

	//! update animations
	static void updateAll(float dt);

	//! cleans all animations
	static void releaseAll();

public:
	//! update sequence, returns false if should be deleted
	virtual bool update(float dt);


protected:
	//! next animation in this sequence
	AnimationSequence* next_;

	//! independent animations
	static std::list<AnimationSequence*> animations_;

	//! time
	float timePassed_;
	float targetTime_;
};


//! one translation animation cant be looped with itself ( it makes no sense )
class AnimationSequenceVector : public AnimationSequence
{
public:
	AnimationSequenceVector(D3DXVECTOR3& position, D3DXVECTOR3 start, D3DXVECTOR3 target, float timeToAnimate);

public:
	//! update sequence, returns false if should be deleted
	virtual bool update(float dt);

protected:

	//! position that we are animating
	D3DXVECTOR3& position_;
	D3DXVECTOR3 start_;
	D3DXVECTOR3 target_;
};

//! one translation animation cant be looped with itself ( it makes no sense )
class AnimationSequenceScalar : public AnimationSequence
{
public:
	AnimationSequenceScalar(float& angle, float start, float target, float timeToAnimate);

public:
	//! update sequence, returns false if should be deleted
	virtual bool update(float dt);

protected:

	//! position that we are animating
	float& position_;
	float start_;
	float target_;
};


//! calls selected function and continue
class AnimationSequenceActivator : public AnimationSequence
{
public:
	AnimationSequenceActivator( FastDelegate0<void> function );

public:
	//! update sequence, returns false if should be deleted
	virtual bool update(float dt);

	//! reset
	virtual void reset()
	{
		AnimationSequence::reset();
		launched_ = false;
	}
protected:
	//!
	FastDelegate0<void> function_;

	bool launched_;
};


//! calls selected function and continue
class AnimationSequenceActivator1Param : public AnimationSequence
{
public:
	AnimationSequenceActivator1Param( FastDelegate1<void*> function, void* param );

public:
	//! update sequence, returns false if should be deleted
	virtual bool update(float dt);

	virtual void reset()
	{
		AnimationSequence::reset();
		launched_ = false;
	}

protected:
	//!
	FastDelegate1<void*> function_;
	void* param_;
	bool launched_;
};


#endif /* ANIMATIONSEQUENCE_H_ */
