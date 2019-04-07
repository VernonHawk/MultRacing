// Made by Igor Morenets; April 2019

#pragma once

#include "Runtime/Core/Public/Math/Vector.h"

struct FHermiteCubicSpline
{
	FVector const StartLocation    { 0.f };
	FVector const StartDerivative  { 0.f };
	FVector const TargetLocation   { 0.f };
	FVector const TargetDerivative { 0.f };
};

inline FVector InterpolateLocation(FHermiteCubicSpline const& Spline, float const LerpRatio)
{
	return FMath::CubicInterp(
		Spline.StartLocation, Spline.StartDerivative,
		Spline.TargetLocation, Spline.TargetDerivative,
		LerpRatio
	);
}

inline FVector InterpolateDerivative(FHermiteCubicSpline const& Spline, float const LerpRatio)
{
	return FMath::CubicInterpDerivative(
		Spline.StartLocation, Spline.StartDerivative,
		Spline.TargetLocation, Spline.TargetDerivative,
		LerpRatio
	);
}
