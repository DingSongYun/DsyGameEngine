#include "cbuffer.h"
#include "vsoutput.hs"

v2p main(a2v input)
{
    v2p output;
    output.position = float4(input.position, 1.0);
    output.color = input.color;

    return output;
}


// Compile command
// fxc /T vs_5_0 /Zi /Fo D3D.vso D3D.vs