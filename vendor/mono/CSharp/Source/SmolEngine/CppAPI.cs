
using System.Runtime.CompilerServices;

namespace SmolEngine
{
    class CppAPI
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GetSetTransformComponent(ref TransformComponent comp, uint ID, bool set);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GetSetHeadComponent(ref HeadComponent comp, uint ID, bool set);
    }
}
