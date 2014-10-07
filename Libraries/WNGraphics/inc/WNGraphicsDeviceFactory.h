////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_GRAPHICS_GRAPHICS_DEVICE_FACTORY_H__
#define __WN_GRAPHICS_GRAPHICS_DEVICE_FACTORY_H__

namespace WNGraphics {
    class WNGraphicsDevice;
    class WNGraphicsResourceFactory;

    class WNGraphicsDeviceFactory {
    public:
        static WNGraphicsDevice* CreateGraphicsDevice(WNGraphicsResourceFactory* _resourceFactory);
    };
}

#endif // __WN_GRAPHICS_GRAPHICS_DEVICE_FACTORY_H__