////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNGraphics/inc/WNGraphicsDeviceFactory.h"
#include "WNGraphics/inc/WNGraphicsDevice.h"
#include "WNMemory/inc/WNAllocation.h"

#ifdef _WN_WINDOWS
    #include "WNGraphics/inc/Internal/D3D11/WNGraphicsDeviceD3D11.h"
#elif defined _WN_LINUX
    #include "WNGraphics/inc/Internal/GLX/WNGraphicsDeviceGLX.h"
#elif defined _WN_ANDROID
    #include "WNGraphics/inc/Internal/EGL/WNGraphicsDeviceEGL.h"
#endif

WNGraphics::WNGraphicsDevice* WNGraphics::WNGraphicsDeviceFactory::CreateGraphicsDevice(WNGraphics::WNGraphicsResourceFactory* _resourceFactory) {
    #ifdef _WN_WINDOWS
        return(WN_NEW WNGraphics::WNGraphicsDeviceD3D11(_resourceFactory));
    #elif defined _WN_LINUX
        return(WN_NEW WNGraphics::WNGraphicsDeviceGLX(_resourceFactory));
    #elif defined _WN_ANDROID
        return(WN_NEW WNGraphics::WNGraphicsDeviceEGL(_resourceFactory));
    #endif
}
