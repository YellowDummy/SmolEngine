using System;

//This is used to setup internal calls
using System.Runtime.CompilerServices;

namespace SmolEngine
{
    public class Game
    {
        //This indicates CSharp that the definition of this method is outside this project, as an Internal Call
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void PrintMethod(string msg);

        static void Main(string[] args)
        {
            Console.WriteLine("This main is being called from CPP");
            PrintMethod("Im executing cpp code from CSharp\n");
        }

    }
}
