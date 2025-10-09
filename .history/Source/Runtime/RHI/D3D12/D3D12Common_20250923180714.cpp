#include "D3D12Common.h"

using Microsoft::WRL::ComPtr;

namespace D3D12Utils
{
	void EnableDebugLayer()
	{
#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
#endif
	}
}