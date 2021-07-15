using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace SmolEngine
{
    class Reflection
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void PushClassName(string name);

        Reflection()
        {
            Type[] typelist = GetTypesInNamespace(Assembly.GetExecutingAssembly(), "SmolEngine");
            for (int i = 0; i < typelist.Length; i++)
            {
                PushClassName(typelist[i].Name);
            }
        }

        private Type[] GetTypesInNamespace(Assembly assembly, string nameSpace)
        {
            return
              assembly.GetTypes()
                      .Where(t => String.Equals(t.Namespace, nameSpace, StringComparison.Ordinal))
                      .ToArray();
        }
    }
}
