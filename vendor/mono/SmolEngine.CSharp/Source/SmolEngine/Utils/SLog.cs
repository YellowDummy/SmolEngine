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
        extern static void AddMessage(string msg, uint level);

        public static void WriteLine(string msg, LogLevel level = LogLevel.Info)
        {
            msg = "[C# Script]: " + msg;
            AddMessage(msg, (uint)level);
        }
    }
}
