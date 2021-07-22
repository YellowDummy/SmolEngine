using System;
using System.Runtime.CompilerServices;

namespace SmolEngine
{
    enum LogLevel
    {
        Info,
        Warn,
        Error
    }

    static class SLog
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void WriteLine_EX(string msg, uint level);

        public static void WriteLine(string msg, LogLevel level = LogLevel.Info)
        {
            msg = "[C# Script]: " + msg;
            WriteLine_EX(msg, (uint)level);
        }
    }
}
