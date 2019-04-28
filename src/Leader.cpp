
#include "consulcpp/Leader.h"
#include "consulcpp/Sessions.h"
#include "consulcpp/Services.h"
#include "consulcpp/Service.h"
#include "consulcpp/Consul.h"

#include "internal/HttpClient.h"

struct consulcpp::Leader::Private
{
	consulcpp::Consul		& mConsul;

	Private( Consul & consul )
		: mConsul( consul )
	{
	}
};

consulcpp::Leader::Leader( Consul & consul ) : d( std::make_unique<Private>( consul ) )
{

}

consulcpp::Leader::~Leader()
{
}

consulcpp::Leader::Status consulcpp::Leader::acquire( const Service & service, const Session & session ) const
{
	internal::HttpClient	restClient;
	Leader::Status			res = Status::Error;
	std::string				query = fmt::format( "{}/{}/kv/service/{}/leader?acquire={}", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion(), service.mName, session.mId );

	auto response = restClient.putAsString( query, {} );

	spdlog::info( "LEADER {} -> <{}>", query, response );

	if( response.find( "true" ) != std::string::npos ){
		res = Status::Yes;
	}else if( response.find( "false" ) != std::string::npos ){
		res = Status::No;
	}
	return res;
}

void consulcpp::Leader::release( const Service & service, const Session & session )
{
	consulcpp::internal::HttpClient		restClient;
	std::string							query = fmt::format( "{}/{}/kv/service/{}/leader?release={}", d->mConsul.agentAddress(), d->mConsul.agentAPIVersion(), service.mName, session.mId );

	auto jsonMaybe = restClient.put( query, {} );

	spdlog::info( "UNLEADER {}", query );

}
