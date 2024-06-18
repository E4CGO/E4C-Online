#include "Sprite.hlsli"

//	スプライト描画単位で指定するパラメーター
cbuffer CbScene : register(b0)
{
	float2	uvScrollValue;	// UVスクロール値
	float2	dummy;			// ダミー
};