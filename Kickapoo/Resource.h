
#pragma once

#define stl_foreach(type, itor, data)	for(type::iterator itor = data.begin(); itor != data.end(); ++itor)

template< typename T > class resource
{
protected:
	static std::list<T*> resourceList;

public:
	resource() {}
	virtual ~resource() {}

	static void releaseResources() {
		stl_foreach(std::list<T*>, i, resourceList) {
			(*i)->release();		
		}
	}

	void addResource() { 
		resourceList.push_back( (T*)this ); 
	}

	void removeResource() {
		resourceList.remove( (T*)this ); 
	}
};

template<typename T> std::list<T*> resource<T>::resourceList;