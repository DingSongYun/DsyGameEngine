#include "D3D12CommandList.h"

D3D12CommandList::~D3D12CommandList()
{
	Shutdown();
}
