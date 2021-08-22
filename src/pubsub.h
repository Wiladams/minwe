#pragma once

#include <memory>
#include <deque>
#include <functional>

/*
	Publish/Subscribe is that typical pattern where a 
	publisher generates interesting data, and a subscriber
	consumes that data.

	The Topic class contains both the publish and subscribe
	interfaces.


	Whatever is responsible for indicating the thing happened
	will call the notify() function of the topic, and the
	subscribed function will be called.

	The Topic does not incoroporate any threading model
	A single topic is not a whole pub/sub system
	Multiple topics are meant to be managed together to create
	a pub/sub system.

	Doing it this way allows for different forms of composition and
	general topic management.

	T - The event payload, this is the type of data that will be
	sent when a subscriber is notified.

	The subscriber is a functor, that is, anything that has the 
	function signature.  It can be an object, or a function pointer,
	essentially anything that resolves as std::function<void>()

	This is a very nice pure type with no dependencies outside
	the standard template library
*/
template <typename T>
class Topic
{
public:
	// This is the form of subscriber
	using Subscriber = std::function<void(const Topic<T>& p, const T m)>;

private:
	std::deque<Subscriber> fSubscribers;

public:
	// Notify subscribers that an event has occured
	// Just do a simple round robin serial invocation
	void notify(const T m)
	{
		for (auto & it : fSubscribers) {
			it(*this, m);
		}
	}

	// Add a subscriber to the list of subscribers
	void subscribe(Subscriber s)
	{
		fSubscribers.push_back(s);
	}

	// Remove a subscriber from the list of subscribers
	void unsubscribe(Subscriber s)
	{
		// BUGBUG
		// find the subscriber that matches
	}
};