// Made by Igor Morenets; April 2019

#pragma once

#include "GameFramework/Actor.h"

inline bool IsLocalClient(AActor const* const Actor)
{
	return Actor && Actor->GetLocalRole() == ROLE_AutonomousProxy;
}

inline bool IsRemoteClient(AActor const* const Actor)
{
	return Actor && Actor->GetLocalRole() == ROLE_SimulatedProxy;
}

inline bool IsServer(AActor const* const Actor)
{
	return Actor && Actor->GetLocalRole() == ROLE_Authority;
}

inline bool IsListenServer(AActor const* const Actor)
{
	return Actor && Actor->GetRemoteRole() == ROLE_SimulatedProxy;
}
