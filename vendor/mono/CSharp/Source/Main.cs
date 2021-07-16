using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace SmolEngine
{
    public class Test
    {
        Test()
        {
            Console.WriteLine("Im created!!");
        }
    }


    public class Names
    {
        Names()
        {
            Type[] typelist = GetTypesInNamespace(Assembly.GetExecutingAssembly(), "SmolEngine");
            for (int i = 0; i < typelist.Length; i++)
            {
                Classes.Add(typelist[i].Name);
                Console.WriteLine(typelist[i].Name);
            }
        }

        public List<string> Classes = new List<string>();

        private Type[] GetTypesInNamespace(Assembly assembly, string nameSpace)
        {
            return
              assembly.GetTypes()
                      .Where(t => String.Equals(t.Namespace, nameSpace, StringComparison.Ordinal))
                      .ToArray();
        }
    }

    public class Game
    {
        static void Main(string[] arg)
        {
            Actor a = new Actor(2222);
            var transform = a.GetComponent<TransformComponent>();
            var head = a.GetComponent<HeadComponent>();

            Console.WriteLine(head.Name);
            Console.WriteLine(head.Tag);
            Console.WriteLine(transform.ToString());
        }

    }
}
