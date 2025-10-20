#pragma once

#include <d3dcommon.h>
#include <string>

bool CompileShaderFromFile(const std::wstring& fileName,
	const std::string& entryPoint,
	const std::string& shaderModel,
	ID3DBlob** ppBlobOut);