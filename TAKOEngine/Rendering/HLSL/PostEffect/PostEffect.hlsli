cbuffer CbPostEffect :register(b0)
{
	float luminanceExtractionLowerEdge;
	float luminanceExtractionHigherEdge;
	float gaussianSigma;
	float bloomIntensity;
}