using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace xnl.xcmp.test
{

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct local_Inforamtion_t
    { 
        ulong  RadioId;
        ushort Zone;
        ushort Channel;
        byte ChannelType;
        byte Reserved01;
        ushort Reserved02;
        ulong Frequency;
    };

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct sample_t
    { 
        ushort Bits;
        ushort Frequency;
    };

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct DateTime_t
    {
        ushort Year;
        byte Month;
        byte Day;
        byte Week;
        byte Hour;
        byte Minute;
        byte Second;
    };

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct VoiceInformation_t
    {
        ulong Index;
        remote_address_t RemoteAddress;
        local_Inforamtion_t LocalInformation;
        byte CallDirection;
        byte CallType;
        ushort Reserved;
        sample_t Sample;
        DateTime_t DateTime;

    };
    class CVoice
    {
    }
}
