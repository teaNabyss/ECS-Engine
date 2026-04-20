#pragma once
#include <memory>
template<typename T>
using Ref = std::shared_ptr<T>;


//links for me to remind what on EARTH this weak pointer is
//https://en.cppreference.com/w/cpp/memory/weak_ptr.html
//https://www.geeksforgeeks.org/cpp/weak_ptr-in-cpp/
//https://cplusplus.com/reference/memory/weak_ptr/weak_ptr/
//https://www.studyplan.dev/pro-cpp/weak-pointers

class Component {
public:
	Component(Component* parent_) :parent(parent_), isCreated(false) {}
	virtual ~Component() = default; /// just means ~Component(){} either will work
	virtual bool OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual void Update(const float deltaTime_) = 0;
	virtual void Render()const = 0;
protected:
	Component* parent;
	/// Just a flag to indicate if the component or actor that inherits this 
	/// base class has called OnCreate (true) 
	bool isCreated;
};