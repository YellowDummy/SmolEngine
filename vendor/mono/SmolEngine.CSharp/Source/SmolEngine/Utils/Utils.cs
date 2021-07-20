using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmolEngine
{
    class Utils
    {
        static public ushort GetComponentType<T>()
        {
            if (typeof(T) == typeof(TransformComponent))
            {
                return (ushort)ComponentTypeEX.Transform;
            }

            return (ushort)ComponentTypeEX.Null;
        }
    }
}
