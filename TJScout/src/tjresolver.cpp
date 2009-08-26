#include "../include/tjresolver.h"
using namespace tj::shared;
using namespace tj::scout;

/** ResolveRequest **/
ResolveRequest::ResolveRequest(const ServiceDescription& type): _sd(type) {
}

ResolveRequest::~ResolveRequest() {
}

const ServiceDescription& ResolveRequest::GetDesiredServiceType() const {
	return _sd;
}

void ResolveRequest::Cancel() {
	EventCancelled.Fire(this, CancelNotification());
}

void ResolveRequest::OnServiceFound(strong<Service> service) {
	{
		ThreadLock lock(&_lock);
		_services[service->GetID()] = service;
	}
	EventService.Fire(this, ServiceNotification(true, service));
}

void ResolveRequest::OnServiceDisappeared(const std::wstring& id) {
	ref<Service> service;

	{
		ThreadLock lock(&_lock);
		std::map<std::wstring, ref<Service> >::iterator it = _services.find(id);
		if(it!=_services.end()) {
			service = it->second;
			_services.erase(it);
		}
	}

	if(service) {
		EventService.Fire(this, ServiceNotification(false, service));
	}
}

ResolveRequest::ServiceNotification::ServiceNotification(bool o, strong<Service> ss): online(o), service(ss) {
}

/** Resolver **/
Resolver::~Resolver() {
}

/** RequestResolver **/
RequestResolver::~RequestResolver() {
}

/** ServiceDescription **/
ServiceDescription::ServiceDescription() {
}

ServiceDescription::~ServiceDescription() {
}

void ServiceDescription::AddType(const ServiceType& type, const std::wstring& data) {
	_description[type] = data;
}

bool ServiceDescription::GetDescriptionOfType(const ServiceType& type, std::wstring& data) const {
	std::map<ServiceType,std::wstring>::const_iterator it = _description.find(type);
	if(it!=_description.end()) {
		data = it->second;
		return true;
	}
	return false;
}