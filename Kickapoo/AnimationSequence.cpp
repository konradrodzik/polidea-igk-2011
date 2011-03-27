/*
 * AnimationSequence.cpp
 *
 *  Created on: 2010-03-14
 *      Author: Mero
 */

#include "Common.h"

std::list<AnimationSequence*> AnimationSequence::animations_;

//-------------------------------------------
AnimationSequence::AnimationSequence()
: next_(NULL)
, timePassed_(0)
, targetTime_(0)
{
}


//-------------------------------------------
AnimationSequence::~AnimationSequence()
{
}


//-------------------------------------------
void AnimationSequence::setNext(AnimationSequence* next)
{
	next_ = next;
}


//-------------------------------------------
void AnimationSequence::add(AnimationSequence* root)
{
	animations_.push_back(root);
}


//-------------------------------------------
void AnimationSequence::remove(AnimationSequence* root)
{
	std::list<AnimationSequence*>::iterator it = animations_.begin();
	while(it != animations_.end())
	{
		if(*it == root)
		{
			animations_.erase(it);
			break;
		}
		it++;
	}

}


//-------------------------------------------
void AnimationSequence::releaseAll()
{
	std::list<AnimationSequence*>::iterator it = animations_.begin();
	std::list<AnimationSequence*>::iterator next;
	while(it != animations_.end())
	{
		next = it;
		next++;
		AnimationSequence* cur = *it;
		AnimationSequence* root = cur;
		if(cur->next_)
		{
			cur = cur->next_;
			while(cur && cur != root)
			{
				AnimationSequence* next = cur->next_;
				delete cur;
				cur = next;
			}
		}
		delete root;
		it = next;
	}
	animations_.clear();
}


//-------------------------------------------
void AnimationSequence::updateAll(float dt)
{
	std::list<AnimationSequence*>::iterator it = animations_.begin();
	std::list<AnimationSequence*>::iterator next;
	while(it != animations_.end())
	{
		AnimationSequence* cur = *it;
		next = it;
		next++;

		AnimationSequence* root;
		root = cur;

		while(cur)
		{
			bool r = cur->update(dt);
			if(!r)
			{
				if(animations_.empty())
					return;

				if(cur->next_)
				{
					cur = cur->next_;
					//! if root then we are looped so reset all
					if(cur == root)
					{
						cur->reset();
						cur = cur->next_;
						while(cur != root)
						{
							cur->reset();
							cur = cur->next_;
						}
						//! loop
						break;
					}
				} else
				{
					//! animation sequence finished and not looped
					delete cur;
					animations_.erase(it);
					break;
				}
			} else
			{
				//! still animating
				break;
			}
		}

		it = next;
	}
}


//-------------------------------------------
bool AnimationSequence::update(float dt)
{
	return false;
}

//-------------------------------------------
void AnimationSequence::reset()
{
	timePassed_ = 0;
}

//-------------------------------------------
AnimationSequenceVector::AnimationSequenceVector(D3DXVECTOR3& position, D3DXVECTOR3 start, D3DXVECTOR3 target, float timeToAnimate)
: position_(position)
, start_(start)
, target_(target)
{
	targetTime_ = timeToAnimate;
}

//-------------------------------------------
bool AnimationSequenceVector::update(float dt)
{
	if(timePassed_ < targetTime_)
	{
		timePassed_ += dt;
		//position_ = start_.lerp(target_, timePassed_ / targetTime_);
		D3DXVec3Lerp(&position_, &start_, &target_, timePassed_ / targetTime_);
	} else
	{
		//! this anim is finished
		return false;
	}

	//! still animating
	return true;
}


//-------------------------------------------
AnimationSequenceScalar::AnimationSequenceScalar(float& position, float start, float target, float timeToAnimate)
: position_(position)
, start_(start)
, target_(target)
{
	targetTime_ = timeToAnimate;
}

//-------------------------------------------
bool AnimationSequenceScalar::update(float dt)
{
	if(timePassed_ < targetTime_)
	{
		timePassed_ += dt;
		position_ = LERP(start_, target_, timePassed_ / targetTime_);

	} else
	{
		//! this anim is finished
		return false;
	}

	//! still animating
	return true;
}


AnimationSequenceActivator::AnimationSequenceActivator( FastDelegate0<void> function )
: function_(function)
, launched_(false)
{}

bool AnimationSequenceActivator::update(float dt)
{
	if(launched_)
		return false;

	launched_ = true;

	if(!function_.empty())
		function_();

	return false;
}


AnimationSequenceActivator1Param::AnimationSequenceActivator1Param( FastDelegate1<void*> function, void* param)
: function_(function)
, param_(param)
, launched_(false)
{}

bool AnimationSequenceActivator1Param::update(float dt)
{
	if(launched_)
		return false;

	if(!function_.empty())
		function_(param_);
	launched_ = true;
	return false;
}

