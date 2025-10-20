#include "ShaderCompileUtils.h"
#include <d3dcompiler.h>
//#include <d3d12.h>
#include <wrl/client.h>
#include "CoreTypes.h"

//#pragma comment(lib, "d3dcompiler.lib")

bool CompileShaderFromFile(const std::wstring& fileName,
	const std::string& entryPoint,
	const std::string& shaderModel,
	ID3DBlob** ppBlobOut)
{
	if (!ppBlobOut) return false;
	*ppBlobOut = nullptr;

	UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(_DEBUG)
	compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3DCompileFromFile(
		fileName.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint.c_str(),
		shaderModel.c_str(),
		compileFlags,
		0,
		ppBlobOut,
		errorBlob.GetAddressOf());

	if (FAILED(hr)) {
		if (errorBlob) {
			// Print compiler errors to debug output (also consider logging to console/file)
			OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
		}
		return false;
	}

	return true;
}